#include "EBYTE.h"
#include "main.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "stm32f0xx_it.h"
#include "usart.h"
#include "lm35.h"
//#include "struct.h"

int main(void) {

	sensor_init();	
	TIM14_init();
	TIM14_interrupt_init();
	
	// configure channel 10 GPIOC I/O-pin 0
	ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_239_5Cycles);
	
	// start the first conversion
	ADC_StartOfConversion(ADC1);
	
//Configure LED3 and LED4 on STM32F0-Discovery
//STM_EVAL_LEDInit(LED3);
//STM_EVAL_LEDInit(LED4);
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	PrintParameters();
	Green_led_init();
	
	while (1) {
		

	/*MyData.Temperature++;
		MyData.hour++;
		MyData.transmitter_ID++;
	
		SendStruct(&MyData, sizeof(MyData));
		Serial_print("Sending ID = ");
		Serial_putint(MyData.transmitter_ID);
		Serial_print(" Hour = ");
		Serial_putint(MyData.hour);
		Serial_print(" Temp = ");
		Serial_putintln(MyData.Temperature);
		timerDelay(MINUTE);*/
	}
}

