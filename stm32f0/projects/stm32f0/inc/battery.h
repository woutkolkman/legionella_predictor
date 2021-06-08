#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "adc.h"
#include "stdbool.h"
#include "debugmode.h"
#include "serial.h" //debug

#define BATTERY_THRESHOLD_VOLTAGE 3140

void battery_transistor_init(void);
void battery_led_init(void);
void battery_status(uint16_t val);

#endif // _BATTERY_H_
