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

//EBYTE LoRa, called when a millisecond has passed and Timer 3 is enabled
void TIM3_IRQHandler(void) {
	time_passed++;
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

//when data is received from LoRa
void USART1_IRQHandler(void) { 

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

// timer to generate 1 second blink
void TIM2_IRQHandler(void) {
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		if (blink) { // generate 1 second blink
			STM_EVAL_LEDOff(LED4); // LED remains off until new temperature measurement
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

// timer to measure temperature every minute
void TIM14_IRQHandler(void) {
	
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) { // wait a minute
		temperature_read_start(); 
	//battery_read_start();
		blink = true; // keep blue LED off; no temperatures are being measured
		if (counter == 60) { // increment counter until hour has passed  
			send = true; // if send = true --> send data (LoRa)
			counter = 0;
		}
		STM_EVAL_LEDOn(LED4); // toggle blue LED for 1 second once a minute
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
  }
}

//ADC sample complete
void ADC1_COMP_IRQHandler(void) {
	
	if (ADC_GetITStatus(ADC1, ADC1_COMP_IRQn) != RESET) {
		//clear interrupt bit
		ADC_ClearITPendingBit(ADC1, ADC1_COMP_IRQn);
		if (adc_battery_meas) {
			//battery measurement
			uint16_t val = ADC_GetConversionValue(ADC1);
			Serial_print("battery: "); //debug
			Serial_putintln(val); //debug
			//battery low? LED on, else off
			if (val > BATTERY_THRESHOLD_VOLTAGE) {
				STM_EVAL_LEDOff(LED4);
			} else {
				STM_EVAL_LEDOn(LED4);
			}
			//TODO transistor pin laagzetten
			adc_battery_meas = false;
		} else {
			Temperatures.Temperature[counter++] = measure_temperature();
		}
	}
}
