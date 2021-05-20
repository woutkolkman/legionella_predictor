#include "green_led.h"
#include "stm32f0xx_gpio.h"
#include <stdint.h>
#include <stdbool.h>

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

// turn green led on
void Green_led_on(void) {
	// set green led high
	GPIOC->BSRR = GPIO_BSRR_BS_9;
}

// turn green led off
void Green_led_off(void) {
	// set green led low
	GPIOC->BSRR = GPIO_BSRR_BR_9;
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
	// determine when a whassing happens
	if( difference(previous_temp, temp) >= TEMP_TRESHOLD && previous_temp > temp) {
		// when wassing happens, turn green led on
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

void Green_led_update_mesure(bool led_on) {
	green_led_status.measure_indication = false;
}

// update green led
void Green_led_update() {
	// set green led
	if(green_led_status.rinse_indication & green_led_status.measure_indication) {
		// when 11
	} else if(green_led_status.rinse_indication & green_led_status.measure_indication) {
		// when 01
	} else if(green_led_status.rinse_indication & green_led_status.measure_indication) {
		// when 01
	} else {
		// when 00
	}
}
