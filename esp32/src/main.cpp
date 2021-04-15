//testmain 0 is the test program for the ESP
//testmain 1 is the ESP LoRa Receiving part
//testmain 2 is the ESP LoRa transmitting part

#define TESTMAIN 1


#if TESTMAIN == 0
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("LED is on");
  delay(1000);
  Serial.println("LED is off");
  delay(1000);
}

#elif TESTMAIN == 1
/*

  This example shows how to connect to an EBYTE transceiver
  using an ESP32

  This code for for the sender

  ESP32 won't allow SoftwareSerial (at least I can't get that lib to work
  so you will just hardwire your EBYTE directly to the Serial2 port



*/

#include "ebyte.h"


/*
WARNING: IF USING AN ESP32
DO NOT USE THE PIN NUMBERS PRINTED ON THE BOARD
YOU MUST USE THE ACTUAL GPIO NUMBER
*/
#define PIN_RX 16   // Serial2 RX (connect this to the EBYTE Tx pin)
#define PIN_TX 17   // Serial2 TX pin (connect this to the EBYTE Rx pin)

#define PIN_M0 4    // D4 on the board (possibly pin 24)
#define PIN_M1 22   // D2 on the board (possibly called pin 22)
#define PIN_AX 21   // D15 on the board (possibly called pin 21)


// i recommend putting this code in a .h file and including it
// from both the receiver and sender modules
struct DATA {
  unsigned long Count;
  int8_t Temperature;
};

// these are just dummy variables, replace with your own
int Chan;
DATA MyData;
unsigned long Last;
uint8_t tempByte;


// create the transceiver object, passing in the serial and pins
EBYTE Transceiver(&Serial2, PIN_M0, PIN_M1, PIN_AX);

void setup() {


  Serial.begin(115200);

  Serial2.begin(9600);
  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Transceiver.init();
  Transceiver.SetMode(MODE_PROGRAM);
  Transceiver.SetUARTBaudRate(UDR_115200);
  Transceiver.SetMode(MODE_NORMAL);
  // all these calls are optional but shown to give examples of what you can do

  // Serial.println(Transceiver.GetAirDataRate());
  // Serial.println(Transceiver.GetChannel());
  // Transceiver.SetAddressH(1);
  // Transceiver.SetAddressL(1);
  // Chan = 15;
  // Transceiver.SetChannel(Chan);
  // save the parameters to the unit,
  // Transceiver.SetPullupMode(1);
  // Transceiver.SaveParameters(PERMANENT);

  // you can print all parameters and is good for debugging
  // if your units will not communicate, print the parameters
  // for both sender and receiver and make sure air rates, channel
  // and address is the same
  Transceiver.PrintParameters();


}

void loop() {

  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()


  if (Transceiver.available()) {

    // i highly suggest you send data using structures and not
    // a parsed data--i've always had a hard time getting reliable data using
    // a parsing method
    //Serial.println(Serial2.read());
    Transceiver.GetStruct(&MyData, sizeof(MyData));

    // note, you only really need this library to program these EBYTE units
    // you can call readBytes directly on the EBYTE Serial object
    // Serial2.readBytes((uint8_t*)& MyData, (uint8_t) sizeof(MyData));
  //Serial.println(tempByte);

    // dump out what was just received
    Serial.print("Count: "); Serial.println(MyData.Count);
    Serial.print("Temp: "); Serial.println(MyData.Temperature);
    //Serial.print("Volts: "); Serial.println(MyData.Volts);
    // if you got data, update the checker
    Last = millis();

  }
  else {
    // if the time checker is over some prescribed amount
    // let the user know there is no incoming data
    if ((millis() - Last) > 1000) {
      Serial.println("Searching: ");
      Last = millis();
    }

  }
}
#elif TESTMAIN == 2
/*

  This example shows how to connect to an EBYTE transceiver
  using an ESP32

  This code for for the sender

  ESP32 won't allow SoftwareSerial (at least I can't get that lib to work
  so you will just hardwire your EBYTE directly to the Serial2 port


*/

#include "ebyte.h"


/*
WARNING: IF USING AN ESP32
DO NOT USE THE PIN NUMBERS PRINTED ON THE BOARD
YOU MUST USE THE ACTUAL GPIO NUMBER
*/
#define PIN_RX 16   // Serial2 RX (connect this to the EBYTE Tx pin)
#define PIN_TX 17   // Serial2 TX pin (connect this to the EBYTE Rx pin)

#define PIN_M0 4    // D4 on the board (possibly pin 24)
#define PIN_M1 22   // D2 on the board (possibly called pin 22)
#define PIN_AX 21   // D15 on the board (possibly called pin 21)


// i recommend putting this code in a .h file and including it
// from both the receiver and sender modules
struct DATA {
  unsigned long Count;
  int Bits;
  float Volts;
  float Amps;

};

// these are just dummy variables, replace with your own
int Chan;
DATA MyData;
unsigned long Last;


// create the transceiver object, passing in the serial and pins
EBYTE Transceiver(&Serial2, PIN_M0, PIN_M1, PIN_AX);

void setup() {


  Serial.begin(115200);
  Serial2.begin(9600);

  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Serial.println(Transceiver.init());
  Transceiver.Reset();
  Transceiver.SetMode(MODE_PROGRAM);
  Transceiver.SetUARTBaudRate(UDR_115200);
  Transceiver.SetMode(MODE_NORMAL);

  // all these calls are optional but shown to give examples of what you can do

  Serial.println(Transceiver.GetAirDataRate());
  Serial.println(Transceiver.GetChannel());
  // Transceiver.SetAddressH(1);
  // Transceiver.SetAddressL(1);
  // Chan = 15;
  // Transceiver.SetChannel(Chan);
  // save the parameters to the unit,
  // Transceiver.SaveParameters(PERMANENT);

  // you can print all parameters and is good for debugging
  // if your units will not communicate, print the parameters
  // for both sender and receiver and make sure air rates, channel
  // and address is the same
  Transceiver.PrintParameters();

}

void loop() {

  // measure some data and save to the structure
  MyData.Count++;
  MyData.Bits = analogRead(A4);
  MyData.Volts = MyData.Bits * ( 5.0 / 1024.0 );

  // i highly suggest you send data using structures and not
  // a parsed data--i've always had a hard time getting reliable data using
  // a parsing method
  //Transceiver.SendStruct(&MyData, sizeof(MyData));
  Serial2.print(0x02);

    // note, you only really need this library to program these EBYTE units
    // you can call write directly on the EBYTE Serial object
    // Serial2.write((uint8_t*) &Data, PacketSize );


  // let the use know something was sent
  Serial.print("Sending: "); Serial.println(0x02);
  delay(1000);


}

#endif