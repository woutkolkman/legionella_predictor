#ifndef _LM35_H_
#define _LM35_H_

#include "stdint.h"
#include "stdbool.h"

// function prototypes
void ADC_init(void);
uint8_t measure_temperature(void);
void temperature_read_start(void);
void TIM14_init(void);
void channel(void);

// global variables
extern bool send;
extern uint8_t counter;

// defines
#define CHANNEL_10 10
#define CHANNEL_11 11

#endif // _LM35_H_
