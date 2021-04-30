/******************************************************************************
 * File           : Main program
 *****************************************************************************/
#include "main.h"

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void) {
	
	// Configure LED3 and LED4 on STM32F0-Discovery
	//STM_EVAL_LEDInit(LED3);
	//STM_EVAL_LEDInit(LED4);
	generate_transmission_id();
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	PrintParameters();
	Green_led_init();
	
	while(1) {
		uint8_t tempprint;
		static bool up;
		
		//read temperature measurement should be added here!
		//STM_EVAL_LEDToggle(LED3);
		//STM_EVAL_LEDToggle(LED4);
		
		//Green_led_update(test_read_tempture());
		//delay(SystemCoreClock/8/10);			//maybe this delay has to be gone, not sure about that, so keeping it.
		
		//temperature rising or falling (just to simulate that)
		if(up) {
			MyData.Temperature++;
		} else {
			MyData.Temperature--;
		}
		
		//simple counter
		MyData.hour++;
		if(MyData.Temperature <= -45) {
			up = true;
		}
		if(MyData.Temperature >= 115) {
			up = false;
		}
		
		//sending the date and showing what is sent.
		SendStruct(&MyData, sizeof(MyData));
		Serial_print("Sending ID: ");
		for(tempprint = 0; tempprint < 8; tempprint++) {
			Serial_putint(MyData.transmitter_ID[tempprint]);
		}
		Serial_print(" Hour: ");Serial_putint(MyData.hour);Serial_print(" = Temp: ");Serial_putintln(MyData.Temperature);
		timerDelay(MINUTE);	
	}
}

void generate_transmission_id() {
	uint8_t count;
	for(count = 0; count < 8; count++) {
		MyData.transmitter_ID[count] = (getTrueRandomNumber() % 256);
	}
}


//https://www.mikrocontroller.net/topic/358453
uint32_t getTrueRandomNumber(void) {
	uint8_t i;
  ADC_InitTypeDef ADC_InitStructure;
  //enable ADC1 clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  // Initialize ADC 14MHz RC
  RCC_ADCCLKConfig(RCC_ADCCLK_HSI14);
  RCC_HSI14Cmd(ENABLE);
  while (!RCC_GetFlagStatus(RCC_FLAG_HSI14RDY))
    ;
  ADC_DeInit(ADC1);
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO; //default
  ADC_Init(ADC1, &ADC_InitStructure);
  //enable internal channel
  ADC_TempSensorCmd(ENABLE);
  // Enable ADCperipheral
  ADC_Cmd(ADC1, ENABLE);
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET)
    ;
  ADC1->CHSELR = 0; //no channel selected
  //Convert the ADC1 temperature sensor, user shortest sample time to generate most noise
  ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor, ADC_SampleTime_1_5Cycles);
  // Enable CRC clock 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
  
  for (i = 0; i < 8; i++) {
    //Start ADC1 Software Conversion
    ADC_StartOfConversion(ADC1);
    //wait for conversion complete
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) {
    }
    CRC_CalcCRC(ADC_GetConversionValue(ADC1));
    //clear EOC flag
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
  }
  //disable ADC1 to save power
  ADC_Cmd(ADC1, DISABLE);
	ADC_DeInit(ADC1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
  return CRC_CalcCRC(0xBADA55E5);
}
