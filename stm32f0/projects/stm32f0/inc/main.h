#include "EBYTE.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include <stdlib.h>

#include "green_led.h"

#define MINUTE 60000 //60000 milliseconds is exactly 1 minute
#define TRANSMITTER_ID_SIZE 8
#define BYTE_MAX_NUMBER 256

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

//the way all data looks like
struct DATA {
	uint8_t transmitter_ID[TRANSMITTER_ID_SIZE];
  uint8_t Temperature;
} Temperatures;


// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void generate_transmission_id(void);
uint32_t get_random_number(void);
void init_random_number(void);
