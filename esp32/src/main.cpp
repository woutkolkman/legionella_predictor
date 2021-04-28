// shape of code inspired by https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino
#include "main.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "website.h"
#include "config_wifi.h"


// set server to listen to port 80
WebServer server(80);

void setup_wifi(bool hotspot,char *ssid, char *password);

// hotspot wifi credentials
const char *hotspot_ssid = "legionella_predetector";
const char *hotspot_password = "zeer_geheim2021";

// init the ESP32
void setup() {
  // set ussart
  Serial.begin(115200); 

  // start wifi hotspot
  setup_wifi(true, (char *) hotspot_ssid,(char *) hotspot_password);

  // add webpages
  server.on("/",homepage);
  server.on("/scanwifi",scan_networks);
  server.on("/connectwifi",connect_to_network);
  server.onNotFound(page_not_found);

  // start to listen to port 80
  Serial.println("HTTP server started on 80");
  server.begin();
}

void loop() { 
  server.handleClient();
  delay(2);
}

