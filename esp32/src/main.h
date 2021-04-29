#include "ebyte.h"

#include <Arduino.h>
#include <WiFi.h>

// contain html code
#include "website.h"

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
  uint8_t transmitter_ID;
  unsigned long hour;
  int8_t Temperature;
} MyData;

//function definition 
void LoRa_get_data(void);
// create the transceiver object, passing in the serial and pins
EBYTE Transceiver(&Serial2, PIN_M0, PIN_M1, PIN_AX);

// network credentials
const char *ssid = "legionella_predetector";
const char *password = "zeer_geheim2021";

// set server to listen to port 80
WiFiServer server(80);