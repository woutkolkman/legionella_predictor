#include "transmitter_id.h"

void generate_transmitter_id(void);
uint32_t get_random_number(void);
void init_random_number(void);
void deInit_random_number(void);

bool exist;


//put an ID in the struct (whether it's existing or new)
void make_transmitter_id() {
	uint8_t loc;
	uint8_t curdata = SE24LC512_ReadData(ID_START_LOCATION);
	if(curdata < '!' || curdata > '~') {
		exist = false;
		for(loc = ID_START_LOCATION; loc < TRANSMITTER_ID_SIZE; loc++) {
			SE24LC512_WriteData(loc, Temperatures.transmitter_ID[loc]);
		}
	} else {
		exist = true;
		for(loc = ID_START_LOCATION; loc < TRANSMITTER_ID_SIZE; loc++) {
			Temperatures.transmitter_ID[loc] = SE24LC512_ReadData(loc);
		}
	}
}

//generates the transmission ID. Saves it in the struct
void generate_transmitter_id() {
	
	uint8_t count;
	init_random_number();
	for(count = 0; count < TRANSMITTER_ID_SIZE; count++) {
		uint8_t number;
		number = (uint8_t) (get_random_number() % MAX_TRANSMISSION_NUMBER);
		if(number < '!') {
			number = number + '!';
		}
		Temperatures.transmitter_ID[count] = number;
	}
	deInit_random_number();
}

//clears the transmitter ID from the eeprom
void clear_transmitter_ID() {
	uint8_t i;
	for(i = 0; i < TRANSMITTER_ID_SIZE; i++) {
		SE24LC512_WriteData(i, 0x00);
		timer_delay(5);
	}
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

//print the current transmitter ID and whether it was new or not
void print_transmitter_id() {
	uint8_t loc;
	if(exist) {
		Serial_print("existing: ");
	} else {
		Serial_print("New: ");
	}
	for(loc = 0; loc < TRANSMITTER_ID_SIZE; loc++) {
		Serial_putint(Temperatures.transmitter_ID[loc]);
		Serial_print(" ");
	}
	Serial_newLine();
}
