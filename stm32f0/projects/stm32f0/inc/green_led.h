#ifndef GREEN_LED_H
#define GREEN_LED_H
#include <stdint.h>
#include <stdbool.h>

#define TEMP_TRESHOLD (1)
#define DELAY_GREEN_TURNOFF (10)

// public defines
void Green_led_init(void);
void Green_led_update(void);
void Green_led_update_rinse(uint8_t);
void Green_led_update_measure(bool);

#endif
