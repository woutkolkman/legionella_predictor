#include "main.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "lm35.h"
#include "battery.h"
#include "struct.h"
#include "transmitter_id.h"
#include "transmission_led.h"

struct DATA Temperatures;

//#define CLEARTRANSMITTERID 0

int main(void) {
	
	I2C_Setup();
	#ifdef CLEARTRANSMITTERID
	clear_transmitter_ID();
	#endif
	make_transmitter_ID();
	
	TIM2_init();
	TIM14_init();
	ADC_init();
	ADC_interrupt_init();
	
	init_transmission_led();
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	set_mode(MODE_PROGRAM);
	
	print_parameters();
	print_transmitter_ID();
	Green_led_init();
	
	STM_EVAL_LEDInit(LED4); // indication if temperatures measurement is ongoing
	
	
	while (1) {
		
		if (send) {
			uint8_t i;
			set_mode(MODE_NORMAL); //sets the LoRa module for transmission
			enable_transmission_led();
			send_struct(&Temperatures, sizeof(Temperatures));
			disable_transmission_led();
			set_mode(MODE_PROGRAM); //sets the LoRa module for sleep mode to save energy
			
			Serial_println("Temperatures: ");
			for (i = 0; i < TEMPERATURE_SIZE; i++) {
				Serial_putint(i);
				Serial_print(" : ");
				Serial_putint(Temperatures.Temperature[i]);
				Serial_println(" degrees.");
			}
			Serial_print("Transmitter ID = ");
			for(i = 0; i < TRANSMITTER_ID_SIZE; i++) {
				Serial_putint(Temperatures.transmitter_ID[i]);
				Serial_char(' ');
			}
			Serial_newLine();
			send = false;
		}
	}
}

//configure interrupt "ADC1_COMP_IRQHandler"
void ADC_interrupt_init(void) {
	
	// Configure ADC ready interrupt
	ADC_ClearITPendingBit(ADC1, ADC1_COMP_IRQn);
	ADC_ITConfig(ADC1, ADC1_COMP_IRQn, ENABLE);
	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	NVIC_SetPriority(ADC1_COMP_IRQn,0);
}
