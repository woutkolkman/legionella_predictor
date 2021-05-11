#include "main.h"
#include "stm32f0xx.h"
#include "lm35.h"
#include "struct.h"

struct DATA Temperatures;

int main(void) {
	generate_transmission_id();
	sensor_init();	
	TIM14_init();
	TIM14_interrupt_init();
	
	// configure channel 10 GPIOC I/O-pin 0
	ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_239_5Cycles);
	
	// start the first conversion
	ADC_StartOfConversion(ADC1);
	
	// Configure LED3 and LED4 on STM32F0-Discovery
	//STM_EVAL_LEDInit(LED3);
	//STM_EVAL_LEDInit(LED4);
	
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	print_parameters();
	Green_led_init();
	while (1) {
	
		int i;
		
		if (send) {
			send_struct(&Temperatures, sizeof(Temperatures));
			Serial_println("Temperatures: ");
			for (i = 0; i < TEMPERATURE_SIZE; i++) {
				Serial_putint(i);
				Serial_print(" : ");
				Serial_putint(Temperatures.Temperature[i]);
				Serial_println(" degrees.");
			}
			Serial_print("Transmitter ID = ");
			for(i = 0; i < TRANSMITTER_ID_SIZE; i++) {
				Serial_putint(Temperatures.transmitter_ID[i]);
			}
			Serial_newLine();
			send = false;
			if (send == false) {
				Serial_println("Waiting for new data...");
			}
		}
	}
}

//generates the transmission ID. Saves it in the struct
void generate_transmission_id() {
	uint8_t count;
	uint8_t number;
	init_random_number();
	for(count = 0; count < 8; count++) {
		number = (uint8_t) (get_random_number() % MAX_TRANSMISSION_NUMBER);
		if(number < '!') {
			number = number + '!';
		}
		Temperatures.transmitter_ID[count] = ((uint8_t) (get_random_number() % MAX_TRANSMISSION_NUMBER));
	}
	deInit_random_number();
}


//https://www.mikrocontroller.net/topic/358453
//creates a random number by ADC values and calculations.
uint32_t get_random_number(void) {
	uint8_t i;
	
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
  return CRC_CalcCRC(0xBADA55E5);
}

//initializes the ADC for the random numbers.
void init_random_number() {
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
}

//deinitializes the ADC for the random numbers
void deInit_random_number() {
	ADC_TempSensorCmd(DISABLE);
	ADC_Cmd(ADC1, DISABLE);
	ADC_DeInit(ADC1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, DISABLE);
	
}
