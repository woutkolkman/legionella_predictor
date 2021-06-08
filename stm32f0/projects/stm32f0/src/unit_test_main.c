#include "stdint.h"
#include "usart.h"
#include "green_led.h"
#include "stm32f0xx_gpio.h"

typedef enum {
	PASSED = 1,
	FAILED = 0
} test_t;

// function that execute a test and print the result
void execute_test(test_t (*test)(void), char *name) {
 test_t result;
	// execute test
	result = test();
	
	// print test titel
	USART_putstr(name);
	USART_putstr(" : ");
	
	// print result of test
	if(result == PASSED) {
		USART_putstr("PASSED");
	}
	else {
		USART_putstr("FAILED");
	}
	USART_putstr("\n");	
}

// test 1
test_t test_U3_Green_led(void) {
 const float values[] = {24,24,23,22,20,20,20,21,21,21,24,24};
 const uint8_t expected[] = {0,0,1,1,1,0,0,0,0,0,0,0}; 
 const uint8_t array_size = 12;
 uint8_t case_number, led_state;
 test_t test_result = PASSED;
	// init 
	Green_led_init();
  Green_led_update_rinse(values[0]);
	Green_led_update();
 
  // execute test
	for(case_number=0; case_number < array_size; case_number++) {
		// print curent case_number
		USART_putstr("case ");
		USART_putint(case_number);
		
		// print current tempature
		USART_putstr(" : temp =");
		USART_putint((int16_t) values[case_number]);
		USART_putstr(" ");
		
		// execute green_led_update()
		Green_led_update_rinse(values[case_number]);	
		Green_led_update();
		
		// read the green led
		led_state = (GPIOC->ODR & GPIO_ODR_9) ? 1 : 0;
		
		// check if output matches the expected output
		USART_putstr("greenled : ");
		if( led_state == expected[case_number]) {
			USART_putint(led_state);
			USART_putstr(" == ");
			USART_putint(expected[case_number]);
			USART_putstr(" PASSED");
		}
		else {
			USART_putint(led_state);
			USART_putstr(" != ");
			USART_putint(expected[case_number]);
			USART_putstr(" FAILED");
			
			// the whole test has failed
			test_result = FAILED;
		}
		
		// newline
		USART_putstr("\n");
  }
 return test_result;	
}

int main() {
	// init usart
	USART_init();
	USART_clearscreen();
	
	// execute tests
	USART_putstr("unit test\n");
	execute_test(test_U3_Green_led, "test of userstory 3AB");
	
	while(1);
}
