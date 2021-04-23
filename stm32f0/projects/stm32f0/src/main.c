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
		#if TRANSCEIVE == 0
		
		GetStruct(&MyData, sizeof(MyData));
		
		
		Serial_print("Count: "); Serial_println(MyData.Count);
    Serial_print("Temp: "); Serial_println(MyData.Temperature);
		
		#elif TRANSCEIVE == 1
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
		
		SendStruct(&MyData, sizeof(MyData));
		Serial_print("Sending Count: ");Serial_putint(MyData.Count);Serial_print(" = Temp: ");Serial_putintln(MyData.Temperature);
		
		#elif TRANSCEIVE == 2
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
		
		#endif
	}
}
