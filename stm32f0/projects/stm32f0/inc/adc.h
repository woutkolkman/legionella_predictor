#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

#define CHANNEL_10 10
#define CHANNEL_11 11

void ADC_interrupt_init(void);
void ADC_init(void);
void ADC_select_channel(uint8_t pin);

#endif // _ADC_H_
