#include "EBYTE.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include "green_led.h"

#define TRANSCEIVE 1
#define MINUTE 60000 // 60000 milliseconds is exactly 1 minute
#define TRANSMITTER_ID 0

// global variables
extern uint16_t rx_read_location;


