#include "stm32f0xx.h"
#include "lm35.h"


/* Zie het kopje LM35 Temperatuursensor --> Proces in technisch ontwerp */
// function to measure current temperature
uint8_t measure_temperature(void) { 
		
	uint16_t adc;
	uint8_t temperature;
	
	// read ADC-value 
	adc = ADC_GetConversionValue(ADC1);
	
	// 12-bits to 10-bits
	adc = adc / 4;
	
	// convert ADC-reading to temperature (degrees Celsius)
	temperature = adc * 0.25;

	return temperature;
}


/* Zie het kopje LM35 Temperatuursensor --> Proces in technisch ontwerp */
// timer to measure temperature every minute 
void TIM14_init(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period      = 500 /*MINUTE*/; 
  TIM_TimeBaseStructure.TIM_Prescaler   = (uint16_t)((SystemCoreClock / 1000) - 1);
	
	NVIC_InitStructure.NVIC_IRQChannel         = TIM14_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // configure time base init
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE); // enable TIM_ITConfig interrupt
	TIM_Cmd(TIM14, ENABLE); // enable interrupt on TIM14
}


/* Zie het kopje Indiatie-LEDs --> Groene LED in technisch ontwerp */
// timer to make green LED light up for 300 ms (washing)
void TIM2_init(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period      = THREE_HUNDRED_MS; // generate 300 ms blink
  TIM_TimeBaseStructure.TIM_Prescaler   = (uint16_t)((SystemCoreClock / 1000) - 1);
	
	NVIC_InitStructure.NVIC_IRQChannel         = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	// configure time base init
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // enable TIM_ITConfig interrupt
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE); 
}
