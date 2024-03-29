#include "green_led.h"

/* Zie het kopje Indiatie-LEDs --> Groene LED in technisch ontwerp */

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
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9;
	
  // Pull-up and pull-down resistors disabled
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR9;
}

void Green_led_set(bool led_on) { // turn green led on
	if(led_on) {
		// set green led high
		GPIOC->BSRR = GPIO_BSRR_BS_9;
	}
	else {
		// set green led low
		GPIOC->BSRR = GPIO_BSRR_BR_9;
	}
}

// calcualte te difference of two numbers
float difference(uint8_t a, uint8_t b) {
	if(a < b) {
		return b-a;
	}
	else {
		return a-b;
	}
}

// set green led on when temprature drops, else set green led off
void Green_led_update_rinse(uint8_t temp) { 
 
	static float previous_temp;
	static int8_t delay = 0;
	
	// determine when rinse happens
	if(difference(previous_temp, temp) >= TEMP_THRESHOLD && previous_temp > temp) {
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

// determine if temperature has been measured
void Green_led_update_measure(bool led_on) { 
	if (led_on) {
		green_led_status.measure_indication = true;
	} else {
		green_led_status.measure_indication = false;
	}
}

// update green led --> update led when temprature dropped / update led when measurement has taken place
void Green_led_update() { 
	Green_led_set( green_led_status.rinse_indication^green_led_status.measure_indication );
}
