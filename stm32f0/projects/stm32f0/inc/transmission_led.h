#include "stm32f0xx.h"
#include "debugmode.h"

#define TRANSMISSION_BUSY_PORT GPIOB
#define TRANSMISSION_BUSY_PIN GPIO_Pin_5

void init_transmission_led(void);
void enable_transmission_led(void);
void disable_transmission_led(void);
