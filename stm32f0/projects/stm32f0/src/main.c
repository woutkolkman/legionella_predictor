#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include "lm35.h"

int main(void) {
	
	USART_init();
	USART_clearscreen();
	sensor_init();
	TIM14_init();
	TIM14_interrupt_init();
	
	// configure channel 10 GPIOC I/O-pin 0
	ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_239_5Cycles);
	
	// start the first conversion
  ADC_StartOfConversion(ADC1);
	
	while (1) {
		
	
	}
}

