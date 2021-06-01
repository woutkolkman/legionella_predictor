#include "green_led.h"
#include "stm32f0_discovery.h"
#include "stm32f0xx_gpio.h"
#include "stdint.h"
#include "stdbool.h"

// Zie het kopje Indiatie-LEDs --> Groene LED in technisch ontwerp 

// private defines
void Green_led_on(void);
void Green_led_off(void);
float difference(uint8_t a, uint8_t b);

// private global variable
struct {
	bool measure_indication;
	bool rinse_indication;
} green_led_status = {false, false};

// setup the green led
void Green_led_init(void) {
	// ----- init the green led
	// green led on gpio port PC9
	// GPIOC Periph clock enable
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 
	
	// set PC9 as output (green led)
  GPIOC->MODER |= GPIO_MODER_MODER9_0;
  // Push pull mode selected
  GPIOC->OTYPER &= ~GPIO_OTYPER_OT_9;
  
	// Maximum speed setting (even though it is unnecessary)
  GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;
	
  // Pull-up and pull-down resistors disabled
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR9;
}

void Green_led_on(void) { // turn green led on
	// set green led high
	GPIOC->BSRR = GPIO_BSRR_BS_9;
}

void Green_led_off(void) { // turn green led off
	// set green led low
	GPIOC->BSRR = GPIO_BSRR_BR_9;
}

float difference(uint8_t a, uint8_t b) { // calcualte te difference of two numbers
	if(a < b) {
		return b-a;
	}
	else {
		return a-b;
	}
}

void Green_led_update_rinse(uint8_t temp) { // set green led on when temprature drops, else set green led off
 
	static float previous_temp;
	static int8_t delay = 0;
	
	// determine when rinse happens
	if(difference(previous_temp, temp) >= TEMP_TRESHOLD && previous_temp > temp) {
		// when rinse happens, turn green led on
		green_led_status.rinse_indication = true;
	}
	else {
		// else if met delay turn green led off
		delay++;
		if(delay > DELAY_GREEN_TURNOFF) { 
			// turn green led off
			green_led_status.rinse_indication = false;
			delay = 0;
		}
	}
  // update previous_temp
  previous_temp = temp;	
}

void Green_led_update_measure(bool led_on) { // determine if temperature has been measured
	
	if (led_on) {
		green_led_status.measure_indication = true;
	} else {
		green_led_status.measure_indication = false;
	}
}

void Green_led_update() { // update green led --> update led when temprature dropped / update led when measurement has taken place

	if(green_led_status.rinse_indication && green_led_status.measure_indication) { // if temperature has been measured and temperature has dropped
		Green_led_off();
	} else if(!green_led_status.rinse_indication && green_led_status.measure_indication) { // if temperature has been measured but temperature has not dropped
		Green_led_on();
	} else if(green_led_status.rinse_indication && !green_led_status.measure_indication) { // if temperature has not been measured and temperature has been dropped
		Green_led_on();
	} else { // in any other case 
		Green_led_off();
	}
}
