#include "stm32f0_discovery.h"
#include "stm32f0xx_gpio.h"
#include "DS18B20.h"

// defines
#define ONEWIRE_OUT  GPIO_Pin_5
#define ONEWIRE_IN   GPIO_Pin_4
#define ONEWIRE_PORT GPIOA

#define ONEWIRE_INPUT_READ  GPIO_ReadInputDataBit(ONEWIRE_PORT, ONEWIRE_IN)
#define ONEWIRE_OUTPUT_HIGH GPIO_WriteBit(GPIOA, ONEWIRE_OUT, Bit_SET)   // 5 V
#define ONEWIRE_OUTPUT_LOW  GPIO_WriteBit(GPIOA, ONEWIRE_OUT, Bit_RESET) // 0 V

#define  READ_ROM					0x33
#define  SKIP_ROM					0xCC
#define  READ_SCRATCHPAD	0xBE
#define  CONVERT_TEMP			0x44
#define  WRITE_SCRATCHPAD	0x4E

// global variables
float temperature_DS = 0;
uint8_t	pad[9];
uint8_t	rom[8];
uint8_t	res[2];

void init_sensor(void) { // sensor on PA4
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}


float check_DS(void) {
	
	init_sensor();
  ping_DS();	
  report_ROM();
  report_scratchpad();
  start_conversion();
	delay_DS(270000);
	report_temperature();

	return temperature_DS;
}

static void ping_DS(void) {
	
	uint8_t	response = 1;
	ONEWIRE_OUTPUT_LOW;
	delay_DS(335);

	while (1) {
		send_init(); // device pulls line low in response
		response = GPIO_ReadInputDataBit(ONEWIRE_PORT, ONEWIRE_IN);
		delay_DS(1512);
		if (response == 0)  
			break;
	}
}

static void start_conversion(void) {

	send_init();
	delay_DS(335);

	send_DS(SKIP_ROM);
	send_DS(CONVERT_TEMP);
}


static void report_temperature(void) {
	
	uint32_t value = 0;
  int i;

	send_init();
	delay_DS(360);

	send_DS(SKIP_ROM);
	send_DS(READ_SCRATCHPAD);
	
	for (i = 0; i < 9; i++) {
		pad[i] = read_DS();
	}
	value = (pad[1] * 256 + pad[0]);			
	temperature_DS = value * (0.0625);  // 12-bit resolution
}

static void report_ROM(void) {
	
	int i;

	send_init();
	delay_DS(335); // 100

	send_DS(READ_ROM);
	for (i = 0; i < 8; i++) {
		rom[i] = read_DS();
	}
}

static void report_scratchpad(void) {
	
	uint8_t	n;

	send_init();
	delay_DS(335); // 100

	send_DS(SKIP_ROM);
	send_DS(READ_SCRATCHPAD);
	
	for (n = 0; n < 9; n++) {
		pad[n] = read_DS();
	}
}

static void send_init(void) {
	
	ONEWIRE_OUTPUT_HIGH;
	delay_DS(1700); // 500

  ONEWIRE_OUTPUT_LOW;
	delay_DS(1700); // 500

	ONEWIRE_OUTPUT_HIGH;
	delay_DS(165); // 50
}

static void send_DS(uint8_t value) {
	
	int	i;

	for (i = 0; i < 8; i++) {
		ONEWIRE_OUTPUT_LOW;
		delay_DS(10); // 5
		
		if (value & 1) { 
			ONEWIRE_OUTPUT_HIGH;
		}
		delay_DS(315); // 95

		ONEWIRE_OUTPUT_HIGH;
		delay_DS(0); //2
		
		value = value >> 1;
	}
}

static uint8_t read_DS(void) {
	
	int	i;
	uint8_t	value = 0;

	for (i = 0; i < 8; i++) {
		value = value >> 1;
		ONEWIRE_OUTPUT_LOW;

		delay_DS(45);// 15
    ONEWIRE_OUTPUT_HIGH;
    delay_DS(20); // 10

		if (ONEWIRE_INPUT_READ) {
			value = value | 0x80;
		}
		delay_DS(198); // 65
	}
	return value;
}

void delay_DS(const int d) {
	
	volatile int i;

	for (i = d; i > 0; i--) { 
		; 
	}
	return;
}
