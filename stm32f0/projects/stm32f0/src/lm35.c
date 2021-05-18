#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "lm35.h"
#include "battery.h"
#include "struct.h"
#include "main.h"

void ADC_init(void) { 
	
	GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef  ADC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // enable clk on ADC1
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
	// configure PC0 + PC1 --> analog mode
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  /* configure the ADC conversion resolution, data alignment, external
  trigger and edge, scan direction and enable/disable the continuous mode
  using the ADC_Init() function. */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;    
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  // calibrate ADC before enabling
  ADC_GetCalibrationFactor(ADC1);

  // activate the ADC peripheral using ADC_Cmd() function.
  ADC_Cmd(ADC1, ENABLE);
	
	// wait until ADC enabled
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET);
}

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

void channel(uint8_t pin) {
	
	uint32_t tmpreg = 0;
	
	ADC_StopOfConversion(ADC1);
	
	ADC1->CHSELR = 0; //no channel selected
	
	if (pin == CHANNEL_10) { //SENSOR
		// configure channel (PC0)
		ADC1->CHSELR |= ADC_CHSELR_CHSEL10; // select CH10
	} else { //BATTERY
		// configure channel (PC1)
		ADC1->CHSELR |= ADC_CHSELR_CHSEL11; // select CH11
	}
	tmpreg &= ~ADC_SMPR1_SMPR;
  tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles;
  ADC1->SMPR = tmpreg;
	
	ADC_StartOfConversion(ADC1);
}

void TIM14_init(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseStructure.TIM_Period      = 100 - 1; 
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

