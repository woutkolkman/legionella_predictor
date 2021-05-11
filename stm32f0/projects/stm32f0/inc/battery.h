#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "stdbool.h"

#define BATTERY_THRESHOLD_VOLTAGE 3140

extern bool adc_battery_meas;

void ADC_battery_init(void);
void battery_read_start(void);
uint16_t battery_read_sync(void);

#endif // _BATTERY_H_
