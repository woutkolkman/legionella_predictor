/**
  ******************************************************************************
  * @file    stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#define RX_BUFFER_SIZE 100
#define NEXT_RX_WRITE_LOCATION ((Rx_write_location + 1) % RX_BUFFER_SIZE)

#include "stm32f0xx_it.h"
#include "lm35.h"
#include "battery.h"
#include "STM32F0_discovery.h"
#include "stdbool.h"
#include "serial.h" //debug

extern volatile unsigned long time_passed;
volatile uint8_t* Rx_buffer;
volatile uint16_t Rx_write_location;
extern uint16_t Rx_read_location;
bool is_full;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
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

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

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

// timer to measure temperature every minute
void TIM14_IRQHandler(void) {
	
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) { // wait a minute
		measure_temperature(); // measure temperature 
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		
		//TODO, measure_temperature() (hierboven) start een conversie en ADC interrupt behandeld de uitkomst
		//start batterijmeting elk uur (counter tot 60) met onderstaande code, en roep daarna sensor_init() enzo weer aan
		/*ADC_battery_init();
		ADC_interrupt_init();
		battery_read_start();*/
  }
}

//ADC sample complete
void ADC1_COMP_IRQHandler(void) {
	
	if(ADC_GetITStatus(ADC1, ADC1_COMP_IRQn) != RESET){
		// Clear interrupt bit
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
			adc_battery_meas = false;
//			sensor_init();
			
		} else {
			//sensor measurement
			//TODO
		}
	}
}
