/******************************************************************************
 * File           : Main program
 *****************************************************************************/
#include "main.h"


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	init_USART2();
	Serial_clearscreen();
	init_LoRa();
	PrintParameters();
	while(1) {
		#if TRANSCEIVE == 2
		if(RxWriteLocation != RxReadLocation || full) {
			Serial_print("byte received: ");Serial_putintln(GetByte());
		}
			
		#elif TRANSCEIVE == 3
		
		static bool up;
		
		if(up) {
			MyData.Temperature++;
		} else {
			MyData.Temperature--;
		}
		MyData.Count++;
		if(MyData.Temperature <= -10) {
			up = true;
		}
		if(MyData.Temperature >= 85) {
			up = false;
		}
		
		SendByte(0x38);
		Serial_print("Sending Byte: 0x38");Serial_newLine();
		//Serial_print("Sending Count: ");Serial_putint(MyData.Count);Serial_print(" = Temp: ");Serial_putintln(MyData.Temperature);
		
		#endif
	}
}

void init_USART2() {
	USART_InitTypeDef USART_Initstructure;
	GPIO_InitTypeDef GPIO_initStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE); 	//periph clock enable
	
	//GPIO for UART
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_initStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_initStructure.GPIO_Pin = USART2_PINS;
	GPIO_initStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_initStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_StructInit(&USART_Initstructure);
	USART_Init(USART2, &USART_Initstructure);
	USART_Cmd(USART2, ENABLE);
}
