#include "main.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include "lm35.h"

int main(void) {

	sensor_init();
	TIM14_init();
	TIM14_interrupt_init();
	
	// configure channel 10 GPIOC I/O-pin 0
	ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_239_5Cycles);
	
	// start the first conversion
	ADC_StartOfConversion(ADC1);
	
	// Configure LED3 and LED4 on STM32F0-Discovery
	//STM_EVAL_LEDInit(LED3);
	//STM_EVAL_LEDInit(LED4);
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	PrintParameters();
	Green_led_init();
	MyData.transmitter_ID = TRANSMITTER_ID;
	
	while(1) {
		static bool up;
		
		//read temperature measurement should be added here!
		//STM_EVAL_LEDToggle(LED3);
		//STM_EVAL_LEDToggle(LED4);
		
		//Green_led_update(test_read_tempture());
		//delay(SystemCoreClock/8/10);			//maybe this delay has to be gone, not sure about that, so keeping it.
		
		//temperature rising or falling (just to simulate that)
		if(up) {
			MyData.Temperature++;
		} else {
			MyData.Temperature--;
		}
		
		//simple counter
		MyData.hour++;
		if(MyData.Temperature <= -45) {
			up = true;
		}
		if(MyData.Temperature >= 115) {
			up = false;
		}
		
		//sending the date and showing what is sent.
		SendStruct(&MyData, sizeof(MyData));
		Serial_print("Sending ID: ");Serial_putint(MyData.transmitter_ID);Serial_print(" Hour: ");Serial_putint(MyData.hour);Serial_print(" = Temp: ");Serial_putintln(MyData.Temperature);
		timerDelay(MINUTE);	
	}
}
