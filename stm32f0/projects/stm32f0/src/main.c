#include "main.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "lm35.h"
#include "battery.h"
#include "struct.h"

struct DATA Temperatures;

int main(void) {
	
	generate_transmission_id();
	TIM2_init();
	TIM14_init();
	battery_led_init();
	battery_transistor_init();
	ADC_init();
	ADC_interrupt_init();
	init_red_led();
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	print_parameters();
	Green_led_init();
	set_mode(MODE_PROGRAM);
	
	while (1) {
		// update green led who turns on when rinseing pipe
		temperature_read_start();
		Green_led_update(measure_temperature());
		Serial_putint(measure_temperature());
		Serial_newLine();

		timer_delay(1000);
		
		if (send) {
			uint8_t i;
			set_mode(MODE_NORMAL); //sets the LoRa module for transmission
			GPIO_SetBits(TRANSMISSION_BUSY_PORT, TRANSMISSION_BUSY_PIN);
			send_struct(&Temperatures, sizeof(Temperatures));
			GPIO_ResetBits(TRANSMISSION_BUSY_PORT, TRANSMISSION_BUSY_PIN);
			set_mode(MODE_PROGRAM); //sets the LoRa module for sleep mode to save energy
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
		}
	}
}

//initializes the red (transmission) led
void init_red_led() {
	GPIO_InitTypeDef GPIO_Initstructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_Initstructure);
}

//generates the transmission ID. Saves it in the struct
void generate_transmission_id() {
	
	uint8_t count;
	
	init_random_number();
	for(count = 0; count < 8; count++) {
		uint8_t number;
		number = (uint8_t) (get_random_number() % MAX_TRANSMISSION_NUMBER);
		if(number < '!') {
			number = number + '!';
		}
		Temperatures.transmitter_ID[count] = number;
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

//configure interrupt "ADC1_COMP_IRQHandler"
void ADC_interrupt_init(void) {
	
	// Configure ADC ready interrupt
	ADC_ClearITPendingBit(ADC1, ADC1_COMP_IRQn);
	ADC_ITConfig(ADC1, ADC1_COMP_IRQn, ENABLE);
	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	NVIC_SetPriority(ADC1_COMP_IRQn,0);
}

//initialize ADC for sensor and battery measurement
void ADC_init(void) { 
	
  ADC_InitTypeDef  ADC_InitStructure;
	
	//enable clocks
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
