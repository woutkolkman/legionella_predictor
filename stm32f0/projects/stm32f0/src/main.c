#include "main.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "lm35.h"
#include "battery.h"
#include "struct.h"
#include "transmitter_id.h"
#include "transmission_led.h"

struct DATA Temperatures;

//#define CLEARTRANSMITTERID 0

int main(void) {
	
	I2C_Setup();
	#ifdef CLEARTRANSMITTERID
	clear_transmitter_ID();
	#endif
	make_transmitter_ID();
	
	TIM2_init();
	TIM14_init();
	battery_led_init();
	battery_transistor_init();
	ADC_init();
	ADC_interrupt_init();
	
	init_transmission_led();
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	set_mode(MODE_PROGRAM);

	print_parameters();
	print_transmitter_ID();
	Green_led_init();
	
	while (1) {
		
		PWR_EnterSleepMode(PWR_SLEEPEntry_WFI); // let STM32 enter sleep mode --> let interrupt handle functions
		
		if (send) {
			uint8_t i;
			set_mode(MODE_NORMAL); //sets the LoRa module for transmission
			GPIOB_enable(); // enable GPIOB clk
			enable_transmission_led();
			send_struct(&Temperatures, sizeof(Temperatures));
			disable_transmission_led();
			GPIOB_disable(); // disable GPIOB clk (not running)
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
				Serial_char(Temperatures.transmitter_ID[i]);
				Serial_char(' ');
			}
			Serial_newLine();
			send = false;
			GPIOA_disable(); // disable GPIOA clock when data has been sent (USART <-> LoRa)
		}
	}
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
