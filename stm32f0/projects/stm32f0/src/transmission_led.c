#include "transmission_led.h"

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

//enables the red transmission led
void enable_transmission_led() {
	
	GPIO_SetBits(TRANSMISSION_BUSY_PORT, TRANSMISSION_BUSY_PIN);
}

//disables the red transmission led
void disable_transmission_led() {
	
	GPIO_ResetBits(TRANSMISSION_BUSY_PORT, TRANSMISSION_BUSY_PIN);
}

// enable GPIOB clk
void GPIOB_enable(void) {
	
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
}

// disable GPIOB clk (not running)
void GPIOB_disable(void) {
	
	RCC->AHBENR &= ~RCC_AHBENR_GPIOBEN;
}
