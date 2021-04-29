// shape of code inspired by https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino
#include "main.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "website.h"
#include "config_wifi.h"
#include "ebyte.h"

// set server to listen to port 80
WebServer interface_server(80);
EBYTE Transceiver(&Serial2, PIN_M0_, PIN_M1_, PIN_AX);
struct DATA {
  uint8_t transmitter_ID;
  unsigned long hour;
  int8_t Temperature;
} MyData;

void setup_wifi(bool hotspot,char *ssid, char *password);

// hotspot wifi credentials
const char *hotspot_ssid = "legionella_predetector";
const char *hotspot_password = "zeer_geheim2021";

// init the ESP32
void setup() {
  //start USART (serial monitor)
  Serial.begin(115200);

  // start wifi hotspot
  setup_wifi(true, (char *) hotspot_ssid,(char *) hotspot_password);

  // add webpages
  interface_server.on("/",homepage);
  interface_server.on("/scanwifi",scan_networks);
  interface_server.on("/connectwifi",connect_to_network);
  interface_server.onNotFound(page_not_found);

  // start to listen to port 80
  Serial.println("HTTP server started on 80");
  interface_server.begin();

  //initialisation of LoRa
  Serial2.begin(9600);
  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Transceiver.init();

}

void loop() { 
  interface_server.handleClient();
  delay(2);

  //THESE LINES HAVE TO BE IMPLEMENTED
  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()
  if (Transceiver.available()) {
    LoRa_get_data();
  }
}

void LoRa_get_data() {
  // i highly suggest you send data using structures and not
    // a parsed data--i've always had a hard time getting reliable data using
    // a parsing method
  Transceiver.GetStruct(&MyData, sizeof(MyData));

  // dump out what was just received
  Serial.print("transmitter_ID: ");Serial.println(MyData.transmitter_ID);
  Serial.print("Hour: "); Serial.println(MyData.hour);
  Serial.print("Temp: "); Serial.println(MyData.Temperature);

}
