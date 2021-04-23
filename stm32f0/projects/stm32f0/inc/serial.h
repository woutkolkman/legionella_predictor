
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_it.h"
//#include <Stream.h>
#include "usart.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
void Serial_char(char c);
void Serial_print(char *str);
void Serial_println(char *str);
void Serial_putint(int16_t v);
void Serial_putintln(int16_t v);
void Serial_clearscreen(void);