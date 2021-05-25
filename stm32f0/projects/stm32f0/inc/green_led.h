#ifndef GREEN_LED_H
#define GREEN_LED_H
#include <stdint.h>
#include <stdbool.h>
#include "debugmode.h"

#define TEMP_TRESHOLD (1)
#define DELAY_GREEN_TURNOFF (1)

// public defines
void Green_led_init(void);
void Green_led_update(void);
void Green_led_update_rinse(uint8_t temp);
void Green_led_update_measure(bool);

#endif
