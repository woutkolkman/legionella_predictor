/******************************************************************************
 * File           : Main program
 *****************************************************************************/
#include "main.h"


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	PrintParameters();
	MyData.transmitter_ID = TRANSMITTER_ID;
	while(1) {
// ----------------------------------------------------------------------------
// if transceive is 0, the receiving part is active
// if transceive is 1, the transmitting part is active
// transceive 2 and transceive 3 were debug settings, 2 receiving 1 byte
// and 3 transmitting 1 byte
// ----------------------------------------------------------------------------
		#if TRANSCEIVE == 0
		
		GetStruct(&MyData, sizeof(MyData));
		
		Serial_print("Count: "); Serial_println(MyData.Count);
    Serial_print("Temp: "); Serial_println(MyData.Temperature);
		
		#elif TRANSCEIVE == 1
		static bool up;
		
		//temperature rising or falling (just to simulate that)
		if(up) {
			MyData.Temperature++;
		} else {
			MyData.Temperature--;
		}
		
		//simple counter
		MyData.hour++;
		if(MyData.Temperature <= -45) {
			up = true;
		}
		if(MyData.Temperature >= 115) {
			up = false;
		}
		
		//sending the date and showing what is sent.
		SendStruct(&MyData, sizeof(MyData));
		Serial_print("Sending ID: ");Serial_putint(MyData.transmitter_ID);Serial_print(" Hour: ");Serial_putint(MyData.hour);Serial_print(" = Temp: ");Serial_putintln(MyData.Temperature);
		timerDelay(MINUTE);
		
		#elif TRANSCEIVE == 2
		if(RxWriteLocation != RxReadLocation || full) {
			Serial_print("byte received: ");Serial_putintln(GetByte());
		}
			
		#elif TRANSCEIVE == 3
		
		SendByte(0x38);
		Serial_print("Sending Byte 0x38");Serial_newLine();
		
		#endif
	}
}
