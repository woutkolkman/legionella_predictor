/*
  The MIT License (MIT)
  Copyright (c) 2019 Kris Kasrpzak
  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  On a personal note, if you develop an application or product using this library 
  and make millions of dollars, I'm happy for you!
*/

/* 
  Code by Kris Kasprzak kris.kasprzak@yahoo.com
  This library is intended to be used with EBYTE transcievers, small wireless units for MCU's such as
  Teensy and Arduino. This library let's users program the operating parameters and both send and recieve data.
  This company makes several modules with different capabilities, but most #defines here should be compatible with them
  All constants were extracted from several data sheets and listed in binary as that's how the data sheet represented each setting
  Hopefully, any changes or additions to constants can be a matter of copying the data sheet constants directly into these #defines
  Usage of this library consumes around 970 bytes
  Revision		Data		Author			Description
  1.0			3/6/2019	Kasprzak		Initial creation
  2.0			3/2/2020	Kasprzak		Added all functions to build the options bit (FEC, Pullup, and TransmissionMode
  3.0			3/27/2020	Kasprzak		Added more Get functions
  4.0			6/23/2020	Kasprzak		Added private method to clear the buffer to ensure read methods would not be filled with buffered data
  5.0			12/4/2020	Kasprzak		moved Reset to public, added Clear to set_mode to avoid buffer corruption during programming
*/

#include <EBYTE.h>
#include <Stream.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/*
create the transciever object
*/

EBYTE::EBYTE(Stream *s, uint8_t PIN_M0, uint8_t PIN_M1, uint8_t PIN_AUX)

{

	_s = s;
	_M0 = PIN_M0;
	_M1 = PIN_M1;
	_AUX = PIN_AUX;

		
}

/*
Initialize the unit--basicall this reads the modules parameters and stores the parameters
for potential future module programming
*/

bool EBYTE::init() {

	bool ok = true;
	
	pinMode(_AUX, INPUT);
	pinMode(_M0, OUTPUT);
	pinMode(_M1, OUTPUT);

	set_mode(MODE_NORMAL);

	// first get the module data (must be called first for some odd reason
	
	ok = read_model_data();

	if (!ok) {
		return false;
	}
	// now get parameters to put unit defaults into the class variables

	ok = read_parameters();
	if (!ok) {
		return false;
	}

	return true;
}


/*
Method to indicate availability
*/

bool EBYTE::available() {

	return _s->available();
	
}

/*
Method to indicate availability
*/

void EBYTE::flush() {

	_s->flush();
	
}

/*
Method to write a single byte...not sure how useful this really is. If you need to send 
more that one byte, put the data into a data structure and send it in a big chunk
*/

void EBYTE::send_byte( uint8_t The_Byte) {

	_s->write(The_Byte);
	
}


/*
Method to get a single byte...not sure how useful this really is. If you need to get 
more that one byte, put the data into a data structure and send/receive it in a big chunk
*/

uint8_t EBYTE::get_byte() {

	return _s->read();

}


/*
Method to send a chunk of data provided data is in a struct--my personal favorite as you 
need not parse or worry about sprintf() inability to handle floats
TTP: put your structure definition into a .h file and include in both the sender and reciever
sketches
NOTE: of your sender and receiver MCU's are different (Teensy and Arduino) caution on the data
types each handle ints floats differently
*/

bool EBYTE::send_struct(const void *the_structure, uint16_t size_) {


		_buf = _s->write((uint8_t *) the_structure, size_);
		
		complete_task(1000);
		
		return (_buf == size_);

}


/*
Method to get a chunk of data provided data is in a struct--my personal favorite as you 
need not parse or worry about sprintf() inability to handle floats
TTP: put your structure definition into a .h file and include in both the sender and reciever
sketches
NOTE: of your sender and receiver MCU's are different (Teensy and Arduino) caution on the data
types each handle ints floats differently
*/


bool EBYTE::get_struct(const void *the_structure, uint16_t size_) {
	
	_buf = _s->readBytes((uint8_t *) the_structure, size_);

	complete_task(1000);

	return (_buf == size_);
}


/*
Utility method to wait until module is doen tranmitting
a time_out is provided to avoid an infinite loop
*/

void EBYTE::complete_task(unsigned long time_out) {

	unsigned long t = millis();

	// make darn sure millis() is not about to reach max data type limit and start over
	if (((unsigned long) (t + time_out)) == 0){
		t = 0;
	}

	// if AUX pin was supplied and look for HIGH state
	// note you can omit using AUX if no pins are available, but you will have to use delay() to let module finish
	if (_AUX != -1) {
		
		while (digitalRead(_AUX) == LOW) {
			
			if ((millis() - t) > time_out){
				break;
			}
		}
	}
	else {
		// if you can't use aux pin, use 4K7 pullup with Arduino
		// you may need to adjust this value if transmissions fail
		delay(1000);

	}


	// per data sheet control after aux goes high is 2ms so delay for at least that long)
	//SmartDelay(20);
	delay(20);
}

/*
method to set the mode (program, normal, etc.)
*/

void EBYTE::set_mode(uint8_t mode) {
	
	// data sheet claims module needs some extra time after mode setting (2ms)
	// most of my projects uses 10 ms, but 40ms is safer

	delay(PIN_RECOVER);
	
	if (mode == MODE_NORMAL) {

		digitalWrite(_M0, LOW);
		digitalWrite(_M1, LOW);
	}
	else if (mode == MODE_WAKEUP) {

		digitalWrite(_M0, HIGH);
		digitalWrite(_M1, LOW);

	}
	else if (mode == MODE_POWERDOWN) {
		digitalWrite(_M0, LOW);
		digitalWrite(_M1, HIGH);
	}
	else if (mode == MODE_PROGRAM) {
		digitalWrite(_M0, HIGH);
		digitalWrite(_M1, HIGH);
	}

	// data sheet says 2ms later control is returned, let's give just a bit more time
	// these modules can take time to activate pins
	delay(PIN_RECOVER);

	// clear out any junk
	// added rev 5
	// i've had some issues where after programming, the returned model is 0, and all settings appear to be corrupt
	// i imagine the issue is due to the internal buffer full of junk, hence clearing
	// Reset() *MAY* work but this seems better.
	clear_buffer();

	// wait until aux pin goes back low
	complete_task(4000);
	
}


// i've asked EBYTE what's supposed to happen--got an unclear answer
// but my testing indicates it clears buffer
// I use this when needing to restart the EBYTE after programming while data is still streaming in
// to let the unit start reading from a cleared internal buffer

// it does NOT return the ebyte back to factory defaults
// if your unit gets corrupt or you need to restore values, you will have to do brute force
// example for and E44-915
// look at the data sheet for default values
//  Trans.set_address_h(0);
//  Trans.set_address_l(0);
//  Trans.set_speed(0b00011100);  
//  Trans.set_channel(1);
//  Trans.set_options(0b01000100);
//  Trans.save_parameters(PERMANENT);


void EBYTE::Reset() {

	
	set_mode(MODE_PROGRAM);

	delay(50); // not sure you need this

	_s->write(0xC4);
	_s->write(0xC4);
	_s->write(0xC4);

	complete_task(4000);

	set_mode(MODE_NORMAL);

}

/*
method to set the speed, 
*/
void EBYTE::set_speed(uint8_t val) {
	_Speed = val;
}
uint8_t EBYTE::get_speed() {
	return _Speed ;
}

/*
method to set options, 
*/
void EBYTE::set_options(uint8_t val) {
	_Options = val;
}
uint8_t EBYTE::get_options() {
	return _Options;
}

/*
method to set the high bit of the address
*/


void EBYTE::set_address_h(uint8_t val) {
	_Address_high = val;
}

uint8_t EBYTE::get_address_h() {
	return _Address_high;
}

/*
method to set the low bit of the address
*/

void EBYTE::set_address_l(uint8_t val) {
	_Address_low = val;
}


uint8_t EBYTE::get_address_l() {
	return _Address_low;
}


/*
method to set the channel
*/

void EBYTE::set_channel(uint8_t val) {
	_Channel = val;
}
uint8_t EBYTE::get_channel() {
	return _Channel;
}


/*
method to set the air data rate
*/

void EBYTE::set_air_data_rate(uint8_t val) {

	_Air_data_rate = val;
	build_speed_byte();
	
}

uint8_t EBYTE::get_air_data_rate() {
	return _Air_data_rate;
}


/*
method to set the parity bit
*/


void EBYTE::set_parity_bit(uint8_t val) {
	_Parity_bit = val;
	build_speed_byte();
}
uint8_t EBYTE::get_parity_bit( ) {
	return _Parity_bit;
}

/*
method to set the options bits
*/

void EBYTE::set_transmission_mode(uint8_t val) {
	_Option_trans = val;
	build_option_byte();
}
uint8_t EBYTE::get_transmission_mode( ) {
	return _Option_trans;
}

void EBYTE::set_pullup_mode(uint8_t val) {
	_Option_pullup = val;
	build_option_byte();
}
uint8_t EBYTE::get_pullup_mode( ) {
	return _Option_pullup;
}

void EBYTE::set_WOR_timing(uint8_t val) {
	_Option_wakeup = val;
	build_option_byte();
}
uint8_t EBYTE::get_WOR_timing() {
	return _Option_wakeup;
}

void EBYTE::set_FEC_mode(uint8_t val) {
	_Option_FEC = val;
	build_option_byte();
}
uint8_t EBYTE::get_FEC_mode( ) {
	return _Option_FEC;
}

void EBYTE::set_transmit_power(uint8_t val) {

	_Option_power = val;
	build_option_byte();

}

uint8_t EBYTE::get_transmit_power() {
	return _Option_power;
}



/*
method to compute the address based on high and low bits
*/

void EBYTE::set_address(uint16_t Val) {
	_Address_high = ((Val & 0xFFFF) >> 8);
	_Address_low = (Val & 0xFF);
}


/*
method to get the address which is a collection of hi and lo bytes
*/


uint16_t EBYTE::get_address() {
	return (_Address_high << 8) | (_Address_low );
}

/*
set the UART baud rate
*/

void EBYTE::set_UART_baud_rate(uint8_t val) {
	_UART_data_rate = val;
	build_speed_byte();
}

uint8_t EBYTE::get_UART_baud_rate() {
	return _UART_data_rate;
}

/*
method to build the byte for programming (notice it's a collection of a few variables)
*/
void EBYTE::build_speed_byte() {
	_Speed = 0;
	_Speed = ((_Parity_bit & 0xFF) << 6) | ((_UART_data_rate & 0xFF) << 3) | (_Air_data_rate & 0xFF);
}


/*
method to build the option byte for programming (notice it's a collection of a few variables)
*/

void EBYTE::build_option_byte() {
	_Options = 0;
	_Options = ((_Option_trans & 0xFF) << 7) | ((_Option_pullup & 0xFF) << 6) | ((_Option_wakeup & 0xFF) << 3) | ((_Option_FEC & 0xFF) << 2) | (_Option_power&0b11);
}


bool EBYTE::get_aux() {
	return digitalRead(_AUX);
}


/*
method to save parameters to the module
*/

void EBYTE::save_parameters(uint8_t val) {
	
	set_mode(MODE_PROGRAM);
	
	// clear_buffer();

	// here you can save permanenly or temp
	delay(5);

	/*
	Serial.print("val: ");
	Serial.println(val);

	Serial.print("_Address_high: ");
	Serial.println(_Address_high);

	Serial.print("_Address_low: ");
	Serial.println(_Address_low);

	Serial.print("_Speed: ");
	Serial.println(_Speed);

	Serial.print("_Channel: ");
	Serial.println(_Channel);

	Serial.print("_Options: ");
	Serial.println(_Options);
	*/


	_s->write(val);
	_s->write(_Address_high);
	_s->write(_Address_low);
	_s->write(_Speed);
	_s->write(_Channel);
	_s->write(_Options);

	delay(50);

	complete_task(4000);
	
	set_mode(MODE_NORMAL);

	
}

/*
method to print parameters, this can be called anytime after init(), because init gets parameters
and any method updates the variables
*/

void EBYTE::print_parameters() {

	_Parity_bit = (_Speed & 0XC0) >> 6;
	_UART_data_rate = (_Speed & 0X38) >> 3;
	_Air_data_rate = _Speed & 0X07;

	_Option_trans = (_Options & 0X80) >> 7;
	_Option_pullup = (_Options & 0X40) >> 6;
	_Option_wakeup = (_Options & 0X38) >> 3;
	_Option_FEC = (_Options & 0X07) >> 2;
	_Option_power = (_Options & 0X03);

	Serial.println("----------------------------------------");
	Serial.print(F("Model no.: "));  Serial.println(_Model, HEX);
	Serial.print(F("Version  : "));  Serial.println(_Version, HEX);
	Serial.print(F("Features : "));  Serial.println(_Features, HEX);
	Serial.println(F(" "));
	Serial.print(F("Mode (HEX/DEC/BIN): "));  Serial.print(_Save, HEX); Serial.print(F("/"));  Serial.print(_Save, DEC); Serial.print(F("/"));  Serial.println(_Save, BIN);
	Serial.print(F("AddH (HEX/DEC/BIN): "));  Serial.print(_Address_high, HEX); Serial.print(F("/")); Serial.print(_Address_high, DEC); Serial.print(F("/"));  Serial.println(_Address_high, BIN);
	Serial.print(F("AddL (HEX/DEC/BIN): "));  Serial.print(_Address_low, HEX); Serial.print(F("/")); Serial.print(_Address_low, DEC); Serial.print(F("/"));  Serial.println(_Address_low, BIN);
	Serial.print(F("Sped (HEX/DEC/BIN): "));  Serial.print(_Speed, HEX); Serial.print(F("/")); Serial.print(_Speed, DEC); Serial.print(F("/"));  Serial.println(_Speed, BIN);
	Serial.print(F("Chan (HEX/DEC/BIN): "));  Serial.print(_Channel, HEX); Serial.print(F("/")); Serial.print(_Channel, DEC); Serial.print(F("/"));  Serial.println(_Channel, BIN);
	Serial.print(F("Optn (HEX/DEC/BIN): "));  Serial.print(_Options, HEX); Serial.print(F("/")); Serial.print(_Options, DEC); Serial.print(F("/"));  Serial.println(_Options, BIN);
	Serial.print(F("Addr (HEX/DEC/BIN): "));  Serial.print(get_address(), HEX); Serial.print(F("/")); Serial.print(get_address(), DEC); Serial.print(F("/"));  Serial.println(get_address(), BIN);
	Serial.println(F(" "));
	Serial.print(F("SpeedParityBit (HEX/DEC/BIN)    : "));  Serial.print(_Parity_bit, HEX); Serial.print(F("/"));  Serial.print(_Parity_bit, DEC); Serial.print(F("/"));  Serial.println(_Parity_bit, BIN);
	Serial.print(F("SpeedUARTDataRate (HEX/DEC/BIN) : "));  Serial.print(_UART_data_rate, HEX); Serial.print(F("/"));  Serial.print(_UART_data_rate, DEC); Serial.print(F("/"));  Serial.println(_UART_data_rate, BIN);
	Serial.print(F("SpeedAirDataRate (HEX/DEC/BIN)  : "));  Serial.print(_Air_data_rate, HEX); Serial.print(F("/"));  Serial.print(_Air_data_rate, DEC); Serial.print(F("/"));  Serial.println(_Air_data_rate, BIN);
	Serial.print(F("OptionTrans (HEX/DEC/BIN)       : "));  Serial.print(_Option_trans, HEX); Serial.print(F("/"));  Serial.print(_Option_trans, DEC); Serial.print(F("/"));  Serial.println(_Option_trans, BIN);
	Serial.print(F("OptionPullup (HEX/DEC/BIN)      : "));  Serial.print(_Option_pullup, HEX); Serial.print(F("/"));  Serial.print(_Option_pullup, DEC); Serial.print(F("/"));  Serial.println(_Option_pullup, BIN);
	Serial.print(F("OptionWakeup (HEX/DEC/BIN)      : "));  Serial.print(_Option_wakeup, HEX); Serial.print(F("/"));  Serial.print(_Option_wakeup, DEC); Serial.print(F("/"));  Serial.println(_Option_wakeup, BIN);
	Serial.print(F("OptionFEC (HEX/DEC/BIN)         : "));  Serial.print(_Option_FEC, HEX); Serial.print(F("/"));  Serial.print(_Option_FEC, DEC); Serial.print(F("/"));  Serial.println(_Option_FEC, BIN);
	Serial.print(F("OptionPower (HEX/DEC/BIN)       : "));  Serial.print(_Option_power, HEX); Serial.print(F("/"));  Serial.print(_Option_power, DEC); Serial.print(F("/"));  Serial.println(_Option_power, BIN);

	Serial.println("----------------------------------------");

}

/*
method to read parameters, 
*/

bool EBYTE::read_parameters() {

	_Params[0] = 0;
	_Params[1] = 0;
	_Params[2] = 0;
	_Params[3] = 0;
	_Params[4] = 0;
	_Params[5] = 0;

	set_mode(MODE_PROGRAM);

	_s->write(0xC1);

	_s->write(0xC1);

	_s->write(0xC1);

	_s->readBytes((uint8_t*)&_Params, (uint8_t) sizeof(_Params));

	_Save = _Params[0];
	_Address_high = _Params[1];
	_Address_low = _Params[2];
	_Speed = _Params[3];
	_Channel = _Params[4];
	_Options = _Params[5];

	_Address =  (_Address_high << 8) | (_Address_low);
	_Parity_bit = (_Speed & 0XC0) >> 6;
	_UART_data_rate = (_Speed & 0X38) >> 3;
	_Air_data_rate = _Speed & 0X07;

	_Option_trans = (_Options & 0X80) >> 7;
	_Option_pullup = (_Options & 0X40) >> 6;
	_Option_wakeup = (_Options & 0X38) >> 3;
	_Option_FEC = (_Options & 0X07) >> 2;
	_Option_power = (_Options & 0X03);
	
	set_mode(MODE_NORMAL);

	if (0xC0 != _Params[0]){
		
		return false;
	}

	return true;
	
}


/*
method to read model data, 
*/
bool EBYTE::read_model_data() {

	_Params[0] = 0;
	_Params[1] = 0;
	_Params[2] = 0;
	_Params[3] = 0;
	_Params[4] = 0;
	_Params[5] = 0;

	bool found = false;
	int i = 0;
	
	set_mode(MODE_PROGRAM);

	_s->write(0xC3);

	_s->write(0xC3);

	_s->write(0xC3);

	_s->readBytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));
	
	_Save = _Params[0];	
	_Model = _Params[1];
	_Version = _Params[2];
	_Features = _Params[3];	

	if (0xC3 != _Params[0]) {

		for (i = 0; i < 5; i++){
			Serial.print("trying: "); Serial.println(i);
			_Params[0] = 0;
			_Params[1] = 0;
			_Params[2] = 0;
			_Params[3] = 0;
			_Params[4] = 0;
			_Params[5] = 0;
			delay(100);

			_s->write(0xC3);

			_s->write(0xC3);

			_s->write(0xC3);

			_s->readBytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));
			
			_Save = _Params[0];	
			_Model = _Params[1];
			_Version = _Params[2];
			_Features = _Params[3];

			if (0xC3 == _Params[0]){
				found = true;
				break;
			}
			
		}
	}
	else {
		found = true;
	}

	set_mode(MODE_NORMAL);

	complete_task(1000);

	return found;
	
}

/*
method to get module model and E50-TTL-100 will return 50
*/

uint8_t EBYTE::get_model() {

	return _Model;
	
}

/*
method to get module version (undocumented as to the value)
*/

uint8_t EBYTE::get_version() {

	return _Version;
	
}

/*
method to get module version (undocumented as to the value)
*/


uint8_t EBYTE::get_features() {

	return _Features;

}

/*
method to clear the serial buffer

without clearing the buffer, i find getting the parameters very unreliable after programming.
i suspect stuff in the buffer affects rogramming 
hence, let's clean it out
this is called as part of the set_mode

*/
void EBYTE::clear_buffer(){

	unsigned long amt = millis();

	while(_s->available()) {
		_s->read();
		if ((millis() - amt) > 5000) {
          Serial.println("runaway");
          break;
        }
	}

}
