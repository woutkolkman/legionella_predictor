#define USART2_PINS (GPIO_Pin_2 | GPIO_Pin_3)
#define BAUDRATE 115200

// ----------------------------------------------------------------------------
// includes
// ----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_it.h"
#include "usart.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------
void init_serial(void);
void Serial_char(char c);
void Serial_print(char *str);
void Serial_println(char *str);
void Serial_putint(int16_t v);
void Serial_putintln(int16_t v);
void Serial_clearscreen(void);
void Serial_newLine(void);
