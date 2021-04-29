#ifndef MAIN_H
#define MAIN_H
#include <WebServer.h>

// defines
/*
WARNING: IF USING AN ESP32
DO NOT USE THE PIN NUMBERS PRINTED ON THE BOARD
YOU MUST USE THE ACTUAL GPIO NUMBER
*/
#define PIN_RX 16   // Serial2 RX (connect this to the EBYTE Tx pin)
#define PIN_TX 17   // Serial2 TX pin (connect this to the EBYTE Rx pin)

#define PIN_M0_ 4    // D4 on the board (possibly pin 24)
#define PIN_M1_ 22   // D2 on the board (possibly called pin 22)
#define PIN_AX 21   // D15 on the board (possibly called pin 21)

// globals
extern WebServer interface_server;

// function definitions 
void LoRa_get_data(void);


#endif