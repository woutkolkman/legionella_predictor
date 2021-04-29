#ifndef _LM35_H_
#define _LM35_H_

#include "stdint.h"

// function prototypes
void sensor_init(void);
uint8_t measure_temperature(void);
void TIM14_init(void);
void TIM14_interrupt_init(void);

#endif // _LM35_H_