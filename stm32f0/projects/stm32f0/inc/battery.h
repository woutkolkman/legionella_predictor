#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "stdbool.h"
#include "serial.h" //debug

#define BATTERY_THRESHOLD_VOLTAGE 3140

extern bool adc_battery_meas;

void transistor_init(void);
void battery_LED_init(void);
void battery_status(bool full);

#endif // _BATTERY_H_
