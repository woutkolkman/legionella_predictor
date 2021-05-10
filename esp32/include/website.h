#ifndef WEBSITE_H
#define WEBSITE_H
#include <stdbool.h>

// function defintions
void homepage();
void page_not_found();
void scan_networks();
void connect_to_network();
void config_cloud_page();

// globals
extern bool mode_is_hotspot;
extern char cloud_address[80];
extern char cloud_port[10];
extern char cloud_path[80];

#endif