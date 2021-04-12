#include "green_led.h"
#include "stm32f0xx_gpio.h"
#include "test_code.c"

// private defines
void Green_led_on(void);
void Green_led_off(void);
float difference(float a, float b);

// setup the green led
void Green_led_init(void)
{
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
void Green_led_on(void)
{
	// set green led high
	GPIOC->BSRR = GPIO_BSRR_BS_9;
}

// turn green led off
void Green_led_off(void)
{
	// set green led low
	GPIOC->BSRR = GPIO_BSRR_BR_9;
}


// calcualte te difference of two numbers
float difference(float a, float b)
{
	if(a < b)
	{
		return b-a;
	}
	else
	{
		return a-b;
	}
}

// set green led on when temprature drops, else set green led off
void Green_led_update(void)
{
 static float previous_temp;
 float temp;
	// vraag tempratuur op
	temp = test_read_tempture();
	
	// determine when a whassing happens
	if( difference(previous_temp, temp) > TEMP_TRESHOLD )
	{
		// when wassing happens, turn green led on
		Green_led_on();
	}
	else
	{
		// else, turn green led off
		Green_led_off();
	}

  // update previous_temp
  previous_temp = temp;	
}
