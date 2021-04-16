//TRANSCEIVE 1 is the ESP LoRa Receiving part
//TRANSCEIVE 2 is the ESP LoRa transmitting part
#define TRANSCEIVE 1

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


struct DATA {
  unsigned long Count;
  int8_t Temperature;
};

// these are just dummy variables, replace with your own
DATA MyData;
unsigned long Last;
#if TRANSCEIVE == 1
bool up;
#endif

// create the transceiver object, passing in the serial and pins
EBYTE Transceiver(&Serial2, PIN_M0, PIN_M1, PIN_AX);
