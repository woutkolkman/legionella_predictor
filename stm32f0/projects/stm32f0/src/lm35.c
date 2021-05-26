#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "lm35.h"
#include "battery.h"
#include "struct.h"
#include "main.h"

uint8_t measure_temperature(void) { // function to measure current temperature
		
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

void temperature_read_start(void) { 
	
  ADC_StartOfConversion(ADC1);
}

void select_channel(uint8_t pin) { // function to select ADC-channel 10/11 (battery and sensor)
	
	uint32_t tmpreg = 0;
	
	ADC_StopOfConversion(ADC1);
	
	ADC1->CHSELR = 0; // no channel selected
	
	if (pin == CHANNEL_10) { // temperature sensor
		// configure channel (PC0)
		ADC1->CHSELR |= ADC_CHSELR_CHSEL10; // select CH10
	} else { // battery
		// configure channel (PC1)
		ADC1->CHSELR |= ADC_CHSELR_CHSEL11; // select CH11
	}
	tmpreg &= ~ADC_SMPR1_SMPR; // clear the sampling time selection bits
  tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles; // set the ADC sampling time register
  ADC1->SMPR = tmpreg; // configure the ADC sample time register
	
	ADC_StartOfConversion(ADC1);
}

void TIM14_init(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  //TIM_TimeBaseStructure.TIM_Period      = DEBUGTIME;
	TIM_TimeBaseStructure.TIM_Period      = 60000 - 1; // minute
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

void TIM2_init(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period      = 300 - 1; // generate 300 ms blink
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


