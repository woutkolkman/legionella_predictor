#ifndef REMEMBER_SETTINGS_H
#define REMEMBER_SETTINGS_H
#include <stdbool.h>

// defines
#define CONFIG_SIGNATURE (0xBD)

// type defintions
typedef struct { // needs to by under 512 bytes
    // !!! The variable signature needs to be te first element of the struct !!!
    uint8_t signature = CONFIG_SIGNATURE; // signature that tells that EEPROM has valid config data(1) = 1 byte
    char cloud_address[80];
    char cloud_port[6]; // max length decimal number for port (5) + \0 (1) = 6 bytes
    char cloud_path[80];
    char wifi_sidd[33]; // max length wifi sidd (32) + \0 (1) = 33 bytes
    char wifi_password[64]; // max length wifi password (63) + \0 (1) = 64 bytes
    bool mode_is_hotspot; // 4 bytes 
} settings_t; // size of settings 268 bytes

// function definitions
void settings_reset(settings_t *settings);
void settings_load(settings_t *settings);
void settings_save(settings_t *settings);
bool settings_is_correct_signature();

#endif
