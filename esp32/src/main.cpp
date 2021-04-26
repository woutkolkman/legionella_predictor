#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// set server to listen to port 80
WebServer server(80);

void setup_wifi(bool hotspot,char *ssid, char *password);

// contain html code
#include "website.h"

// network credentials
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
  server.onNotFound(pageNotFound);


  // start to listen to port 80
  Serial.println("HTTP server started on 80");
  server.begin();
}

void loop() { 
  server.handleClient();
  delay(2);
}

void setup_wifi(bool hotspot,char *ssid, char *password) {
 IPAddress IP;
  // decide to make a hotspot or connect to a network
  if(hotspot == true) { // make wifi hotspot
    // setup a wifi access point
    Serial.printf("Start a wifi accesspoint : %s \n",ssid);
    WiFi.softAP(ssid, password);

    // get ip address of esp32
    IP = WiFi.softAPIP();

    // set status flag
    mode_is_hotspot = true;
  }
  else { // connect to a wifi network
    // setup wifi
    WiFi.mode(WIFI_STA); // mode = station mode
    WiFi.begin(ssid, password); // initialize WiFi using networkname + password

    while (WiFi.status() != WL_CONNECTED) { // wait while connected has not been made yet
      Serial.print(".");
      delay(1000);
    }
    
    // get ip adress of esp32
    IP = WiFi.localIP();

    // set status flag
    mode_is_hotspot = false;
  }

  // print ip adress to serial
  Serial.print("\nIP address of ESP32 : ");
  Serial.println(IP);

  // begin mDNS
  if (MDNS.begin("test")) {
    Serial.println("MDNS responder started");
    MDNS.addService("pretector","HTTP",80);
  }
}