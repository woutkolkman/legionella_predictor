#define RX_BUFFER_SIZE 200
#define NEXT_RXWRITE_LOCATION ((RxWriteLocation + 1) % RX_BUFFER_SIZE)

#include "stm32f0xx_it.h"
#include "stdbool.h"
#include "lm35.h"
#include "struct.h"
#include "serial.h"

extern volatile unsigned long timehad;
volatile uint8_t* RxBuffer;
volatile uint16_t RxWriteLocation;
extern uint16_t RxReadLocation;
bool full;

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

void TIM3_IRQHandler(void) {
	timehad++;
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

void USART1_IRQHandler(void) { 
	// Read Data Register not empty interrupt?
  if(USART1->ISR & USART_ISR_RXNE) {
		if(NEXT_RXWRITE_LOCATION == RxReadLocation) {						//last location of the buffer will be filled, setting bool to let it know it's full.
			RxBuffer[RxWriteLocation] = USART1->RDR;
			RxWriteLocation = NEXT_RXWRITE_LOCATION;
			full = true;
		} else if (RxWriteLocation == RxReadLocation && full) {	//the buffer is full, but a new character is there, throw away this character as there is no space left
			USART1->RDR; //throw away data, no space left
		} else {																								//Just add the received data to the buffer, everything is fine
			RxBuffer[RxWriteLocation] = USART1->RDR;
			RxWriteLocation = NEXT_RXWRITE_LOCATION;
		}
	}
}
	
void TIM14_IRQHandler(void) { // timer to measure temperature every minute
	
	uint8_t data;
	
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) { // wait a minute
		MyData.Temperature = measure_temperature();
	//SendStruct(&MyData, sizeof(MyData));
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
  }
}

