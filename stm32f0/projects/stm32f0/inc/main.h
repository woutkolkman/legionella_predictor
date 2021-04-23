#include "EBYTE.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"

#define TRANSCEIVE 1


// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
extern uint16_t RxReadLocation;


struct DATA {
  unsigned long Count;
  int8_t Temperature;
} MyData;


// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------

