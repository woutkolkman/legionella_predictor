/******************************************************************************
 * File           : USART driver header file
******************************************************************************/
#ifndef _USART_H_
#define _USART_H_

#include "stdint.h"

/******************************************************************************
  Function prototypes
******************************************************************************/
void USART_init(void);
void USART_putc(char c);
char USART_getc(void);
void USART_putstr(char *str);
void USART_putint(int16_t v);
void USART_getstr(char *str);
void USART_clearscreen(void);

#endif // _USART_H_
