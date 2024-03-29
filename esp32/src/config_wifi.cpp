/* zie kopje "Uitleg individuele componenten>ESP32 (gateway)>Wifi" in technisch ontwerp */
#include "config_wifi.h"

// globals
DNSServer DNS_server;

// setup hotspot or connect to a network
void setup_wifi(bool hotspot,char *ssid, char *password) {
 IPAddress IP;
  // reset wifi to prevent errors
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  // decide to make a hotspot or connect to a network
  if(hotspot == true) { // make wifi hotspot
    // turn mode to Access Point
    WiFi.mode(WIFI_AP);

    // setup IP, gateway and mask
    IP = IPAddress(192,168,1,1);
    WiFi.softAPConfig(IP, IP, IPAddress(255,255,255,0));
    WiFi.softAP(ssid, password);

    // DNS give by every request theportal
    /* zie kopje "Uitleg individuele componenten>ESP32 (gateway)>captive portal" in technisch ontwerp */
    DNS_server.start(DNS_PORT, "*", IP);
  }
  else { // connect to a wifi network
    // setup wifi
    WiFi.mode(WIFI_STA); // mode = station mode
    WiFi.begin(ssid, password); // initialize WiFi using networkname + password
    while (WiFi.status() != WL_CONNECTED) { // wait while connected has not been made yet
      Serial.print('.');
      delay(1000);
      if(Serial.available()) {
        if(Serial.read() == 'r') {
          Serial.println("Default hotspot mode.");
          settings_reset(&settings);
          setup_wifi(settings.mode_is_hotspot, (char *) settings.wifi_sidd,(char *) settings.wifi_password);
          return;
        }
      }
    }
    
    // get ip adress of esp32
    IP = WiFi.localIP();

    // set status flag
    settings.mode_is_hotspot = false;
  }

  // print ip adress to serial
  Serial.print("\nIP address of ESP32 : ");
  Serial.println(IP);
} 
