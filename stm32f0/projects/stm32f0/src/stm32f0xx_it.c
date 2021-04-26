#include "stm32f0xx_it.h"
#include "lm35.h"
#include "STM32F0_discovery.h"
#include "usart.h"

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

void USART1_IRQHandler(void)
{
}

void USART2_IRQHandler(void)
{
}

void TIM14_IRQHandler(void) { // timer to measure temperature every minute
	
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) { // wait a minute
		measure_temperature(); // measure temperature 
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
  }
}

