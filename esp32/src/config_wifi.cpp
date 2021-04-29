#include "config_wifi.h"
#include "website.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// setup hotspot or connect to a network
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
}
