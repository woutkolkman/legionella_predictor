#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include "green_led.h"

#include "test_code.c"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void delay(const int d);

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	// Configure LED3 and LED4 on STM32F0-Discovery
	//STM_EVAL_LEDInit(LED3);
	//STM_EVAL_LEDInit(LED4);
	USART_init();
	Green_led_init();
	
	while(1) {
		//STM_EVAL_LEDToggle(LED3);
		//STM_EVAL_LEDToggle(LED4);
		Green_led_update(test_read_tempture());
		delay(SystemCoreClock/8/10);
}

