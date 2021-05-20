#include "stm32f0xx_it.h"
#include "lm35.h"
#include "battery.h"
#include "stm32f0_discovery.h"
#include "stdbool.h"
#include "serial.h" //debug
#include "stdbool.h"
#include "lm35.h"
#include "serial.h"
#include "struct.h"

#define RX_BUFFER_SIZE 100
#define NEXT_RX_WRITE_LOCATION ((Rx_write_location + 1) % RX_BUFFER_SIZE)

extern volatile unsigned long time_passed;
volatile uint8_t* Rx_buffer;
volatile uint16_t Rx_write_location;
extern uint16_t Rx_read_location;
bool is_full;
uint8_t counter = 0;
bool send = false;
bool adc_battery_meas; //false --> sensor measurement

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


void TIM1_BRK_UP_TRG_COM_IRQHandler(void) { // EBYTE LoRa, called when a millisecond has passed and Timer 1 is enabled
	time_passed++;
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

void USART1_IRQHandler(void) { // when data is received from LoRa

	// Read Data Register not empty interrupt?
  if(USART1->ISR & USART_ISR_RXNE) {
		if(NEXT_RX_WRITE_LOCATION == Rx_read_location) {						//last location of the buffer will be filled, setting bool to let it know it's is_full.
			Rx_buffer[Rx_write_location] = USART1->RDR;
			Rx_write_location = NEXT_RX_WRITE_LOCATION;
			is_full = true;
		} else if (Rx_write_location == Rx_read_location && is_full) {	//the buffer is full, but a new character is there, throw away this character as there is no space left
			USART1->RDR; //throw away data, no space left
		} else {																								//Just add the received data to the buffer, everything is fine
			Rx_buffer[Rx_write_location] = USART1->RDR;
			Rx_write_location = NEXT_RX_WRITE_LOCATION;
		}
	}
}

void TIM14_IRQHandler(void) { // timer to measure temperature every minute
	
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) { // wait a minute
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		if (counter >= TEMPERATURE_SIZE) { // every hour
			adc_battery_meas = true; // check battery-voltage every hour
			send = true; // if send = true --> send data (LoRa)
			counter = 0;
		}
		select_channel(CHANNEL_10); // select ADC-channel 10 for temperature measurements
  }
}

void ADC1_COMP_IRQHandler(void) { // ADC sample complete
	
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
			//previous measurement from sensor
			Temperatures.Temperature[counter++] = measure_temperature(); // if ADC-channel 10 is selected --> 60 temperature measurements
		}
		
		if (adc_battery_meas) { // do battery measurement --> select ADC-channel 11
			GPIOC->BRR = GPIO_Pin_6; //enable transistor
			select_channel(CHANNEL_11);
		}
	}
}
