#include "transmission_led.h"

/* Zie het kopje Indiatie-LEDs --> Rode LED in technisch ontwerp */

//initializes the red (transmission) led
void init_transmission_led() {
	
	GPIO_InitTypeDef GPIO_Initstructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Initstructure.GPIO_Pin = TRANSMISSION_BUSY_PIN;
	GPIO_Init(GPIOB, &GPIO_Initstructure);
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, DISABLE);
}

//enables or disables the red transmission led
void transmission_led_cmd(bool led_on) {
	if(led_on) {
		GPIO_SetBits(TRANSMISSION_BUSY_PORT, TRANSMISSION_BUSY_PIN);
	} else {
		GPIO_ResetBits(TRANSMISSION_BUSY_PORT, TRANSMISSION_BUSY_PIN);
	}
}

/* Zie het kopje Energiezuinigheid --> GPIO-B clock in technisch ontwerp */
// enable GPIOB clk
void GPIOB_enable(void) { 
	
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
}

/* Zie het kopje Energiezuinigheid --> GPIO-B clock in technisch ontwerp */
// disable GPIOB clk (not running)
void GPIOB_disable(void) { 
	
	RCC->AHBENR &= ~RCC_AHBENR_GPIOBEN;
}
