#include "main.h"
#include "stm32f0xx.h"
#include "lm35.h"
#include "struct.h"

struct DATA MyData;

int main(void) {
	
	sensor_init();	
	TIM14_init();
	TIM14_interrupt_init();
	
	// configure channel 10 GPIOC I/O-pin 0
	ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_239_5Cycles);
	
	// start the first conversion
	ADC_StartOfConversion(ADC1);

	init_serial();
	Serial_clearscreen();
	init_LoRa();
	PrintParameters();
	Green_led_init();
	
	while (1) {
	
		int i;
		
		if (send) {
			SendStruct(&MyData, sizeof(MyData));
			for (i = 0; i < TEMPERATURE_SIZE; i++) {
				Serial_putint(i);
				Serial_print(" : ");
				Serial_putint(MyData.Temperature[i]);
				Serial_println(" ");
			}
			Serial_print("Transmitter ID = ");
			Serial_putintln(MyData.transmitter_ID);
			Serial_print("Hour = ");
			Serial_putintln(MyData.hour);
			send = false;
			if (send == false) {
				Serial_println("Waiting for new data...");
			}
		}
	}
}

