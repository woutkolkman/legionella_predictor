#include <EEPROM.h>
#include <string.h>
#include "remember_settings.h"

// reset settings to standard settings
void settings_reset(settings_t *settings) {
    // default cloud settings
    strcpy(settings->cloud_address, "http://145.44.235.205");
    strcpy(settings->cloud_port, "80");
    strcpy(settings->cloud_path,"/packet");

    // default wifi settings
    strcpy(settings->wifi_sidd,"Legionella Predictor GateStef");
    strcpy(settings->wifi_password, "zeer_geheim2021");
    settings->mode_is_hotspot = true;

    // save default settings to EEPROM
    settings_save(settings);    
}

// load settings from EEPROM
void settings_load(settings_t *settings) {
    EEPROM.begin(sizeof(settings_t));

    EEPROM.readBytes(0,settings, sizeof(settings_t));
    EEPROM.commit();

    EEPROM.end();
}

// save settings to EEPROM
void settings_save(settings_t *settings) {
    EEPROM.begin(sizeof(settings_t));

    EEPROM.writeBytes(0,settings, sizeof(settings_t));
    EEPROM.commit();

    EEPROM.end();
}

// check if first byte from EEPROM match correct signature
bool settings_is_correct_signature() {
  uint8_t signature;
    EEPROM.begin(sizeof(settings_t));

    // read signiture from EEPROM
    signature = EEPROM.readByte(0);

    EEPROM.end();

    // check if the signature is valid
    if( signature == CONFIG_SIGNATURE) {
        return true;
    }
    else {
        return false;
    }
}
