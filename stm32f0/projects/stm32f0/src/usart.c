/******************************************************************************
 * File           : USART driver implementation file
 *****************************************************************************/
#include "stm32f0xx.h"
#include "usart.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
volatile char rx_buffer;

//comment the following line to test on the laptop instead of on the STM32
#define ONTARGET 1

#ifndef ONTARGET

#include <stdio.h>

//test functions for use without USART and without STM32 (testing on laptop)
void USART_init()           { }
void USART_putc(char c)     { printf( "%c", c ); }
void USART_putstr( char* s) { printf( "%s", s ); }
void USART_putint(int16_t v)  { printf( "%d", v ); }
char USART_getc(void)       { return( 'x' ); }
void USART_getstr(char *str){ str[0] = 0; }
void USART_clearscreen()    { }

#else

void USART_init(void)
{
  // GPIOA Periph clock enable
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  
  // PA9 and PA10 Alternate function mode
  GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
  
  // Set alternate functions AF1 for PA9 and PA10
  GPIOA->AFR[1] |= 0x00000110;
  
  // USART1 clock enable
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN; 

  // 115200 Bd @ 48 MHz
  // USARTDIV = 48 MHz / 115200 = 416 = 0x01A0
  // BRR[15:4] = USARTDIV[15:4]
  // When OVER8 = 0, BRR [3:0] = USARTDIV [3:0]
  USART1->BRR = 0x01A0;

  // USART enable
  // Receiver enable
  // Transmitter enable
  USART1->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;

  // Default value
  USART1->CR2 = 0;
  USART1->CR3 = 0; 
}

void USART_putc(char c)
{
  // Wait for Transmit data register empty
  while((USART1->ISR & USART_ISR_TXE) == 0) ;

  // Transmit data by writing to TDR, clears TXE flag  
  USART1->TDR = c;
}

void USART_putstr(char *str)
{
  while(*str)
  {
    if(*str == '\n')
    {
      USART_putc('\r');
    }
    
    USART_putc(*str++);
  }
}

void USART_putint(int16_t v)
{
  static char str[10];
	
  // first convert an integer to an ASCII string of characters, terminated with a NULL
  // v: the number to convert. Negative numbers are also allowed.
  // p: pointer to the destination buffer
  int16_t t1 = 0, t2 = 0;
  char h[10];

  if (v < 0)
  {
    str[t2] = '-';
    t2++;
    v = -v;
  }
  
  do
  {
    h[t1] = v % 10 + 48;
    t1++;
    v = v / 10;

  } while (v > 0);

  while (t1 > 0)
  {
    str[t2++] = h[--t1];
  }

  str[t2] = '\0';
  USART_putstr(str);
}

char USART_getc(void)
{
  char c;

  // Was there an Overrun error?
  if((USART1->ISR & USART_ISR_ORE) != 0)
  {
    // Yes, clear it 
    USART1->ICR |= USART_ICR_ORECF;
  }

  // Wait for data in the Receive Data Register
  while((USART1->ISR & USART_ISR_RXNE) == 0) ;

  // Read data from RDR, clears the RXNE flag
  c = (char)USART1->RDR;

  return(c);
}

void USART_getstr(char *str)
{
  // Implement this function yourself
}

// Implements the following VT100 terminal commands
// - Clear screan
// - Cursor home
void USART_clearscreen(void)
{
  char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
  char cmd2[4] = {0x1B, '[', 'f', '\0'}; // Cursor home
  
  USART_putstr(cmd1);
  USART_putstr(cmd2);
}

#endif
