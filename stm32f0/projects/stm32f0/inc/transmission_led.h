#include "stm32f0xx.h"
#include <stdbool.h>

#define TRANSMISSION_BUSY_PORT GPIOB
#define TRANSMISSION_BUSY_PIN GPIO_Pin_5

void init_transmission_led(void);
void transmission_led_cmd(bool led_on);
void GPIOB_enable(void);
void GPIOB_disable(void);
