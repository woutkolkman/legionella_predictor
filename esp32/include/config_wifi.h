#ifndef CONFIG_WIFI_H
#define CONFIG_WIFI_H
#include <DNSServer.h>
#include "website.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "main.h"

// defines
#define DNS_PORT (53)

// globals
extern DNSServer DNS_server;

// function definitions
void setup_wifi(bool hotspot,char *ssid, char *password);

#endif