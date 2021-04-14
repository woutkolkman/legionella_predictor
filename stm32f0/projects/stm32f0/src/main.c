#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "DS18B20.h"
#include "usart.h"

int main(void) {
	
	USART_init();
	USART_clearscreen();
	
	while (1) {
		
	  char string[100];
    float temperature = check_DS();
	  USART_itoa(temperature, string);
	  USART_putstr(string);
		
 // check_DS();
	}
}

