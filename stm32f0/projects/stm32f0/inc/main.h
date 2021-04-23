#include "EBYTE.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"

#define TRANSCEIVE 1
#define USART2_PINS (GPIO_Pin_2 | GPIO_Pin_3)

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

struct DATA {
  unsigned long Count;
  int8_t Temperature;
} MyData;


// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void init_USART2(void);
