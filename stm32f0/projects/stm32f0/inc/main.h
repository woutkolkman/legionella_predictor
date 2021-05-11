#include "EBYTE.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include "stdlib.h"
#include "green_led.h"

// defines
#define BYTE_MAX_NUMBER 256

// function prototypes
void generate_transmission_id(void);
uint32_t get_random_number(void);
void init_random_number(void);
