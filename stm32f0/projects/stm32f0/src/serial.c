#include "serial.h"

void Serial_char(char c) {
  // Wait for Transmit data register empty
  while((USART2->ISR & USART_ISR_TXE) == 0) ;

  // Transmit data by writing to TDR, clears TXE flag  
  USART2->TDR = c;
}

void Serial_print(char *str) {
  while(*str)
  {
    if(*str == '\n')
    {
     Serial_char('\r');
    }
    Serial_char(*str++);
  }
}

void Serial_println(char *str) {
	Serial_print(str);
	Serial_char('\n');
	Serial_char('\r');
}

void Serial_putint(int16_t v) {
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
  Serial_print(str);
}

void Serial_putintln(int16_t v) {
	Serial_putint(v);
	Serial_char('\n');
	Serial_char('\r');
}
void Serial_clearscreen(void)
{
  char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
  char cmd2[4] = {0x1B, '[', 'f', '\0'}; // Cursor home
  
  Serial_println(cmd1);
  Serial_println(cmd2);
}

