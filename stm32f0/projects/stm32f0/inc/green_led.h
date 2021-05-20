#ifndef GREEN_LED_H
#define GREEN_LED_H
#include <stdint.h>

#define TEMP_TRESHOLD (1)
#define DELAY_GREEN_TURNOFF (10)

// public defines
void Green_led_init(void);
void Green_led_update(uint8_t);

#endif
