#include "battery.h"
#include "lm35.h"
#include "struct.h"

bool adc_battery_meas; //false --> sensor measurement

void ADC_battery_init(void) {
	
	ADC_InitTypeDef  ADC_InitStructure;
	
	ADC_DeInit(ADC1);
	STM_EVAL_LEDInit(LED4); //init battery-low LED
//ADC_VrefintCmd(ENABLE); //enable 1,8V internal reference voltage
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // enable clk on ADC1
	
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
	
	// configure channel 11 GPIOC I/O-pin 1
	ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_239_5Cycles);
}

//start battery measurement, handle in ISR
void battery_read_start(void) {

	//TODO transistor pin hoogzetten
	adc_battery_meas = true;
	ADC_StartOfConversion(ADC1);
}


//busy wait read ADC
uint16_t battery_read_sync(void) {
	
	ADC_StartOfConversion(ADC1);
	
	//wait until conversion ready
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	return ADC_GetConversionValue(ADC1);
}
