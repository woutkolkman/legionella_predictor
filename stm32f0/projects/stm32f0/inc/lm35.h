#ifndef _LM35_H_
#define _LM35_H_

#include "stdint.h"
#include "stdbool.h"
#include "adc.h"


// defines
#define MINUTE 					 (60000 - 1)
#define THREE_HUNDRED_MS (300 - 1)

// function prototypes
uint8_t measure_temperature(void);
void TIM14_init(void);
void TIM2_init(void);

// global variables
extern bool send;
extern uint8_t counter;
extern bool blink;

#endif // _LM35_H_
