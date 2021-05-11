#ifndef CONFIG_WIFI_H
#define CONFIG_WIFI_H
#include <DNSServer.h>

// defines
#define DNS_PORT (53)

// globals
extern DNSServer DNS_server;

// function definitions
void setup_wifi(bool hotspot,char *ssid, char *password);

#endif