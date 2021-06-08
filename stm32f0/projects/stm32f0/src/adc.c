#include "adc.h"


/* Zie het kopje LoRa communicatie --> Temperatures in technisch ontwerp 
	 Zie het kopje Elektronisch schema sensor-systeem -> Temperatuursensor en batterij in technisch ontwerp */
// configure interrupt "ADC1_COMP_IRQHandler"
void ADC_interrupt_init(void) { 
	
	// Configure ADC ready interrupt
	ADC_ClearITPendingBit(ADC1, ADC1_COMP_IRQn);
	ADC_ITConfig(ADC1, ADC1_COMP_IRQn, ENABLE);
	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	NVIC_SetPriority(ADC1_COMP_IRQn,0);
}


/* Zie het kopje LoRa communicatie --> Temperatures in technisch ontwerp 
	 Zie het kopje Elektronisch schema sensor-systeem -> Temperatuursensor en batterij in technisch ontwerp */ 
// initialize ADC for sensor and battery measurement
void ADC_init(void) { 
	
  ADC_InitTypeDef  ADC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
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
	
	// Wait for the ADC to be ready!
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));
	
	ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_239_5Cycles);
}


/* Zie het kopje Elektronisch schema sensor-systeem -> Temperatuursensor en batterij in technisch ontwerp */
// function to select ADC-channel 10/11 (battery and sensor)
void ADC_select_channel(uint8_t pin) { 
	
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
