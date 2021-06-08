#include "stm32f0xx_it.h"
#include "lm35.h"
#include "battery.h"
#include "stm32f0_discovery.h"
#include "stdbool.h"
#include "stdbool.h"
#include "lm35.h"
#include "struct.h"
#include "green_led.h"
#include "usart.h"

// global variables
extern volatile unsigned long time_passed;
volatile uint8_t* Rx_buffer;
volatile uint16_t Rx_write_location;
bool is_full;
uint8_t counter = 0;
bool send = false;
bool adc_battery_meas; // false --> sensor measurement
bool blink = false;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

// EBYTE LoRa, called when a millisecond has passed and Timer 1 is enabled
void TIM1_BRK_UP_TRG_COM_IRQHandler(void) { 
	time_passed++;
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/* Zie het kopje Indicatie-LEDs --> Groene LED in technisch ontwerp */
// timer to generate 300 ms blink
void TIM2_IRQHandler(void) { 
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		if (blink) { // generate 300 ms blink
			Green_led_update_measure(false); // LED remains off until new temperature measurement
			Green_led_update(); 
			TIM_Cmd(TIM2, DISABLE); // disable TIM2 and clk; more efficient 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
		}	
	}
}

/* Zie het kopje LoRa communicatie --> Temperatures in technisch ontwerp 
	 Zie het kopje Indicatie-LEDs --> Groene LED in technisch ontwerp
	 Zie het kopje Energiezuinigheid --> GPIO-A CLOCK in technisch ontwerp 
	 Zie het kopje Elektronisch schema sensor-systeem -> Temperatuursensor en batterij in technisch ontwerp */
// timer to measure temperature every minute
void TIM14_IRQHandler(void) { 
  
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) { // wait a minute
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		Green_led_update_measure(true); // toggle green LED on for 300 ms once a minute
		blink = true; // start blink (300 ms)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		TIM_Cmd(TIM2, ENABLE);
		Green_led_update(); // update green led
		if (counter >= TEMPERATURE_SIZE) { // for every hour
			GPIOA_enable(); // enable GPIOA clock --> send data to gateway (USART <-> LoRa)
			adc_battery_meas = true; // check battery-voltage every hour
			send = true; // if send = true --> send data (LoRa)
			counter = 0;
		}
		ADC_select_channel(CHANNEL_10); // select ADC-channel 10 for temperature measurements
  }
}

/* Zie het kopje LoRa communicatie --> Temperatures in technisch ontwerp 
	 Zie het kopje Elektronisch schema sensor-systeem -> Temperatuursensor en batterij in technisch ontwerp */
// ADC sample complete
void ADC1_COMP_IRQHandler(void) { 
	
	if (ADC_GetITStatus(ADC1, ADC1_COMP_IRQn) != RESET) {
		//clear interrupt bit
		ADC_ClearITPendingBit(ADC1, ADC1_COMP_IRQn);
		
		if (ADC1->CHSELR & ADC_CHSELR_CHSEL11) {
			//previous measurement from battery
			uint16_t val = ADC_GetConversionValue(ADC1); // battery measurement
			
			GPIOC->BSRR = GPIO_Pin_6; //disable transistor
			adc_battery_meas = false;
			
			//battery low? flash LED, else LED off
			battery_status(val);
		} else {
			Temperatures.Temperature[counter++] = measure_temperature(); // if ADC-channel 10 is selected --> 60 temperature measurements
			Green_led_update_rinse(Temperatures.Temperature[counter-1]); // update rinse green led indication
		}
		if (adc_battery_meas) { // do battery measurement --> select ADC-channel 11
			GPIOC->BRR = GPIO_Pin_6; //enable transistor
			ADC_select_channel(CHANNEL_11);
		}
	}
}
