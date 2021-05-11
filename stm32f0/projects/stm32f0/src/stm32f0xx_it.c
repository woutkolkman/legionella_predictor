#define RX_BUFFER_SIZE 100
#define NEXT_RX_WRITE_LOCATION ((Rx_write_location + 1) % RX_BUFFER_SIZE)

#include "stm32f0xx_it.h"
#include "lm35.h"
#include "battery.h"
#include "STM32F0_discovery.h"
#include "stdbool.h"
#include "serial.h" //debug
#include "struct.h"

extern volatile unsigned long time_passed;
volatile uint8_t* Rx_buffer;
volatile uint16_t Rx_write_location;
extern uint16_t Rx_read_location;
bool is_full;
bool send = false;
uint8_t counter = 0;

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

// timer to measure temperature every minute
void TIM14_IRQHandler(void) {
	
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) { // wait a minute
		Temperatures.Temperature[counter++] = measure_temperature();
		if (counter == 60) {
			temperature_read_start();
		//ADC_battery_init();
		//battery_read_start();
			counter = 0;
		}
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		
		//TODO, temperature_read_start() (hierboven) start een conversie en ADC interrupt behandeld de uitkomst
		//start batterijmeting elk uur (counter tot 60) met onderstaande code, en roep daarna sensor_init() enzo weer aan
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
			sensor_init();
		} else {
			//sensor measurement
			Serial_println("Measuring temperatures...");
			send = true; // if send = true --> send data (LoRa)
		}
	}
}
