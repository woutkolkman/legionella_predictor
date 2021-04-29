/******************************************************************************
 * File           : Main program
 *****************************************************************************/
#include "main.h"

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void) {
	
	// Configure LED3 and LED4 on STM32F0-Discovery
	//STM_EVAL_LEDInit(LED3);
	//STM_EVAL_LEDInit(LED4);
	init_serial();
	Serial_clearscreen();
	init_LoRa();
	PrintParameters();
	Green_led_init();
	MyData.transmitter_ID = TRANSMITTER_ID;
	while(1) {
		static bool up;
		
		//read temperature measurement should be added here!
		//STM_EVAL_LEDToggle(LED3);
		//STM_EVAL_LEDToggle(LED4);
		
		//Green_led_update(test_read_tempture());
		//delay(SystemCoreClock/8/10);			//maybe this delay has to be gone, not sure about that, so keeping it.
		
		//temperature rising or falling (just to simulate that)
		if(up) {
			MyData.Temperature++;
		} else {
			MyData.Temperature--;
		}
		
		//simple counter
		MyData.hour++;
		if(MyData.Temperature <= -45) {
			up = true;
		}
		if(MyData.Temperature >= 115) {
			up = false;
		}
		
		//sending the date and showing what is sent.
		SendStruct(&MyData, sizeof(MyData));
		Serial_print("Sending ID: ");Serial_putint(MyData.transmitter_ID);Serial_print(" Hour: ");Serial_putint(MyData.hour);Serial_print(" = Temp: ");Serial_putintln(MyData.Temperature);
		timerDelay(MINUTE);	
	}
}
