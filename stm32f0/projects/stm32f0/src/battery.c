#include "battery.h"

/* Zie Elektronisch schema sensor-systeem --> transistor in technisch ontwerp */
//init transistor output pin
void battery_transistor_init(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIOC->BSRR = GPIO_Pin_6; //block open drain
}


/* Zie Indicatie-LEDs --> Blauwe LED in technisch ontwerp */
//init battery-low LED
void battery_led_init(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;  
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_1);
}

/* Zie Indicatie-LEDs --> Blauwe LED in technisch ontwerp */
//enable flashing battery-low LED
void timer3_init(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //enable TIM3 clk
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period        = FIVE_HUNDRED_MS;
	TIM_TimeBaseStructure.TIM_Prescaler     = (SystemCoreClock / 1000) - 1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//channel 3 (BLUE LED)
	TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse       = 0;
	TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
	
	TIM_OC3Init(TIM3, &TIM_OCInitStructure); // channel for LED_BLUE
	
	TIM_Cmd(TIM3, ENABLE); // enable TIMx timer
}

/* Zie Indicatie-LEDs --> Blauwe LED in technisch ontwerp */
//disable flashing battery-low LED
void timer3_deinit(void) {

	TIM_Cmd(TIM3, DISABLE);
	TIM_DeInit(TIM3);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
	GPIOC->BRR = GPIO_Pin_8; //LED off
}

/* Zie Indicatie-LEDs --> Blauwe LED in technisch ontwerp */
//turn LED-flashing on or off
void battery_status(uint16_t val) {
	
	static bool was_full = true;
	
	if (val > BATTERY_THRESHOLD_VOLTAGE) {
		if (!was_full) {
			timer3_deinit();
		}
		was_full = true;	
	} else {
		if (was_full) {
			timer3_init();
		}
		was_full = false;
	}
}
