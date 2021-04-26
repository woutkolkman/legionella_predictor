#ifndef _LM35_H_
#define _LM35_H_

// defines
#define ONE_MINUTE 60

// function prototypes
void sensor_init(void);
float measure_temperature(void);
void delay(const int d);
void TIM14_init(void);
void TIM14_interrupt_init(void);

#endif // _LM35_H_
