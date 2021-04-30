// ----------------------------------------------------------------------------
// Serial is used on usart2
// Using usart2, it will not be in the way with usart1, which is connected to LoRa
// ----------------------------------------------------------------------------
#include "serial.h"

// ----------------------------------------------------------------------------
// initialize pins and usart
// ----------------------------------------------------------------------------
void init_serial() {
	USART_InitTypeDef USART_Initstructure;
	GPIO_InitTypeDef GPIO_initStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE); 	//periph clock enable
	
	//GPIO for UART2
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_initStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_initStructure.GPIO_Pin = USART2_PINS;
	GPIO_initStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_initStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_StructInit(&USART_Initstructure);	//using default values.
	USART_Initstructure.USART_BaudRate = BAUDRATE;
	USART_Init(USART2, &USART_Initstructure);
	
	USART_Cmd(USART2, ENABLE);
}

//prints a character
void Serial_char(char c) {
  // Wait for Transmit data register empty
  while((USART2->ISR & USART_ISR_TXE) == 0) ;

  // Transmit data by writing to TDR, clears TXE flag  
  USART2->TDR = c;
}

//print a string
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

//print a string, after the string, start on a new line
void Serial_println(char *str) {
	Serial_print(str);
	Serial_newLine();
}


//function to print an integer
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

//print an integer, and start on a new line
void Serial_putintln(int16_t v) {
	Serial_putint(v);
	Serial_newLine();
}


//clears the screen
void Serial_clearscreen(void)
{
  char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
  char cmd2[4] = {0x1B, '[', 'f', '\0'}; // Cursor home
  
  Serial_println(cmd1);
  Serial_println(cmd2);
}

//puts the cursor on a new line
void Serial_newLine() {
	Serial_print("\n\r");
}
