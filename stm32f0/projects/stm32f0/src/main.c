#include "main.h"

struct DATA Temperatures;

int main(void) {
	
	I2C_Setup();
	#ifdef CLEARTRANSMITTERID
	clear_transmitter_ID();
	#endif
	make_transmitter_ID();
	
	TIM2_init();
	TIM14_init();
	battery_led_init();
	battery_transistor_init();
	ADC_init();
	ADC_interrupt_init();
	
	init_transmission_led();
	init_LoRa();
	set_mode(MODE_PROGRAM);
	Green_led_init();
	
	while (1) {
		
		/* Zie het kopje Energiezuinigheid --> Sleep-modus in technisch ontwerp */
		PWR_EnterSleepMode(PWR_SLEEPEntry_WFI); // let STM32 enter sleep mode --> let interrupt handle functions
		
		if (send) {
			set_mode(MODE_NORMAL); // sets the LoRa module for transmission
			GPIOB_enable(); // enable GPIOB clk
			transmission_led_cmd(true); 
			send_struct(&Temperatures, sizeof(Temperatures)); /* Zie het kopje LM35 Temperatuursensor --> Proces in technisch ontwerp 
																													 Zie het kopje LoRa communicatie --> Proces --> Verzenden (STM32) in technisch ontwerp */
			transmission_led_cmd(false);
			GPIOB_disable(); // disable GPIOB clk (not running)
			set_mode(MODE_PROGRAM); //sets the LoRa module for sleep mode to save energy
			send = false;
			GPIOA_disable(); // disable GPIOA clock when data has been sent (USART <-> LoRa)
		}
	}
}
