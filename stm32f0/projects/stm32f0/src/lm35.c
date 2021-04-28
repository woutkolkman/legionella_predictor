#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "lm35.h"

void sensor_init(void) { // initialization for analog temperature sensor (LM35)
	
	GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef  ADC_InitStructure;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // enable clk on ADC1
  
  // GPIOC --> pin 0 configuration
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* configure the ADC conversion resolution, data alignment, external
  trigger and edge, scan direction and enable/disable the continuous mode
  using the ADC_Init() function. */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
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
	
	// convert ADC-reading to temperature (degrees Celsius)
	temperature = adc * 0.25;

	return temperature;
}

void TIM14_init(void) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseStructure.TIM_Period      = 60000 - 1; 
  TIM_TimeBaseStructure.TIM_Prescaler   = (uint16_t)((SystemCoreClock / 1000) - 1);
  
  // configure time base init
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
}

void TIM14_interrupt_init(void) {
	
	NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel         = TIM14_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
   
  TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE); // enable TIM_ITConfig interrupt
  TIM_Cmd(TIM14, ENABLE); // enable interrupt on TIM14
}
