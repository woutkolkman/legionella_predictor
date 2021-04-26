#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// set server to listen to port 80
WebServer server(80);

// contain html code
#include "website.h"

// network credentials
const char *ssid = "legionella_predetector";
const char *password = "zeer_geheim2021";

// init the ESP32
void setup() {
  // set ussart
  Serial.begin(115200); 

  // setup a wifi access point
  Serial.printf("Start a wifi accesspoint : %s \n",ssid);
  WiFi.softAP(ssid, password);

  // get ip address for esp32
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // begin mDNS
  if (MDNS.begin("test")) {
    Serial.println("MDNS responder started");
  }

  // add webpages
  server.on("/",homepage);
  server.on("/scanwifi",scan_networks);
  server.on("/connectwifi",connect_to_network);
  server.onNotFound(pageNotFound);


  // start to listen to port 80
  Serial.println("HTTP server started on 80");
  server.begin();
}

void loop() { 
  server.handleClient();
  delay(2);
}