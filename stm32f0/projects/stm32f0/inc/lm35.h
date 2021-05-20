#ifndef _LM35_H_
#define _LM35_H_

#include "stdint.h"
#include "stdbool.h"

#define HOUR (60000 - 1)
#define DEBUGTIME (500 - 1) //30 seconds
// function prototypes
uint8_t measure_temperature(void);
void temperature_read_start(void);
void TIM14_init(void);
void select_channel(uint8_t pin);

// global variables
extern bool send;
extern uint8_t counter;

// defines
#define CHANNEL_10 10
#define CHANNEL_11 11

#endif // _LM35_H_
