#include "EBYTE.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include <stdlib.h>

#include "green_led.h"


#define TRANSCEIVE 1
#define MINUTE 60000 //60000 milliseconds is exactly 1 minute
#define TRANSMITTER_ID_SIZE 8

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
extern uint16_t rx_read_location;

//the way all data looks like
struct DATA {
	uint8_t transmitter_ID[8];
  unsigned long hour;
  uint8_t Temperature;
} MyData;


// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void generate_transmission_id(void);
uint32_t getTrueRandomNumber(void);
