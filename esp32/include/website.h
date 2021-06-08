#ifndef WEBSITE_H
#define WEBSITE_H
#include <stdbool.h>
#include <Arduino.h>
#include <iostream>
#include <string>
#include "main.h"
#include "config_wifi.h"
#include <WebServer.h>

// function defintions
void homepage();
void page_not_found();
void scan_networks();
void connect_to_network();
void config_cloud_page();

#endif