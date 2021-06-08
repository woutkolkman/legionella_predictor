#ifndef __STM32F0XX_IT_H
#define __STM32F0XX_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f0xx.h"
#include "lm35.h"
#include "battery.h"
#include "stm32f0_discovery.h"
#include "stdbool.h"
#include "lm35.h"
#include "struct.h"
#include "green_led.h"
#include "usart.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif 

