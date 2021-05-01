#include "main.h"
#include "stm32f0xx.h"
#include "lm35.h"
#include "struct.h"

struct DATA Temperatures;

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
			SendStruct(&Temperatures, sizeof(Temperatures));
			Serial_println("Temperatures: ");
			for (i = 0; i < TEMPERATURE_SIZE; i++) {
				Serial_putint(i);
				Serial_print(" : ");
				Serial_putint(Temperatures.Temperature[i]);
				Serial_println(" degrees.");
			}
			Serial_print("Transmitter ID = ");
			Serial_putintln(Temperatures.transmitter_ID);
			Serial_print("Hour = ");
			Serial_putintln(Temperatures.hour);
			send = false;
			if (send == false) {
				Serial_println("Waiting for new data...");
			}
		}
	}
}

