// ----------------------------------------------------------------------------
// Code for the EBYTE LoRa module.
// ----------------------------------------------------------------------------
#include <EBYTE.h>

// Zie kopje LoRa communicatie --> Initialisatie --> STM32 in technisch ontwerp

// global variables. putting them in the 'EBYTE.h' files gives 'multiply defined' errors. 
uint16_t Rx_read_location;
uint8_t *Tx_buffer;

// pin variables
int8_t _M0;
int8_t _M1;
int8_t _AUX;

// variable for the 6 bytes that are sent to the module to program it
// or bytes received to indicate modules programmed settings
uint8_t _Params[6];

// indicidual variables for each of the 6 bytes
// _Params could be used as the main variable storage, but since some bytes
// are a collection of several options, let's just make storage consistent
// also Param[1] is different data depending on the _Save variable
uint8_t _Save;
uint8_t _Address_High;
uint8_t _Address_low;
uint8_t _Speed;
uint8_t _Channel;
uint8_t _Options;
	
// individual variables for all the options
uint8_t _Parity_bit;
uint8_t _UART_data_rate;
uint8_t _Air_data_rate;
uint8_t _Option_transmission;
uint8_t _Option_pullup;
uint8_t _Option_wakeup;
uint8_t _Option_FEC;
uint8_t _Option_power;
uint16_t _Address;
uint8_t _Model;
uint8_t _Version;
uint8_t _Features;
uint8_t _buf;

volatile unsigned long time_passed;

//global initialisation function for the lora module
bool init_LoRa() {
	bool ok;
	init_LoRa_GPIO();
	init_Timer_Delay();
	init_USART();
	
	set_mode(MODE_NORMAL);
	
	ok = read_model_data();
	
	if(!ok) {
		return false;
	}
	
	ok = read_parameters();
	if(!ok) {
		return false;
	}
	
	return true;
}

//initializes the pinouts of M0, M1 and AUX (Rx and Tx will be in usart)
void init_LoRa_GPIO() {
	GPIO_InitTypeDef GPIO_initStructure;
	
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_initStructure.GPIO_Pin = (LORA_M0_PIN | LORA_M1_PIN);
	GPIO_Init(GPIOA, &GPIO_initStructure);
	
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_initStructure.GPIO_Pin = LORA_AUX_PIN;
	GPIO_Init(GPIOA, &GPIO_initStructure);
}

//initializes USART1
void init_USART() {
	USART_InitTypeDef USART_Initstructure;
	init_USART_GPIO();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_StructInit(&USART_Initstructure);
	USART_Init(USART1, &USART_Initstructure);
	init_buffer();
	#ifdef DMA 
	init_DMA_write();
	
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	#else
	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);
	USART_disable();
	#endif
	
}

//initializes the GPIO pins for usart1
void init_USART_GPIO() {
	GPIO_InitTypeDef GPIO_initStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE); 	//periph clock enable
	
	//GPIO for UART
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_initStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_initStructure.GPIO_Pin = LORA_USART1_PINS;
	GPIO_initStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_initStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
}
#ifdef DMA
//initializes the DMA
void init_DMA_write() {
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	// DMA channel Tx of USART Configuration
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART1->TDR);
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t) Tx_buffer;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize         = TX_BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable; 
  
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	
}
#endif

//allocates memory for the buffers
void init_buffer() {
	Rx_buffer = (uint8_t*)malloc(sizeof(uint8_t)*RX_BUFFER_SIZE);
	if(Rx_buffer == NULL) {
		//Error handling;
		HardFault_Handler();
	}
	Tx_buffer = (uint8_t*)malloc(sizeof(uint8_t)*TX_BUFFER_SIZE);
	if(Tx_buffer == NULL) {
		//Error handling;
		HardFault_Handler();
	}
}

//initializes the timer used for delays
void init_Timer_Delay() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Period = TIM1PERIOD;				//48000000 / 1000 / 48 = 1000 = 1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = TIM1PRESCALE;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
}

//checks whether Data is received through LoRa
bool available() {
	return(USART1->ISR & USART_ISR_RXNE);
}

//sends a byte through LoRa
void send_byte( uint8_t the_byte) {
	USART_putc(the_byte);
	complete_task(1000);
}

//receives a byte through LoRa
uint8_t get_byte() {
	uint8_t result = Rx_buffer[Rx_read_location];
	Rx_read_location = NEXT_RX_READ_LOCATION;
	is_full = false;
	complete_task(1000);
	return result;
}

void send_struct(const void* the_structure, uint16_t size) { // sends a struct through LoRa
  uint16_t count = 0;
	uint8_t *C_The_Structure = (uint8_t *) the_structure;
	Tx_buffer = C_The_Structure;
	USART_enable();
	while (count < size) {
	  USART_putc(Tx_buffer[count]);
		count++;
	}
	complete_task(1000);
	USART_disable();
}

//receiving / reading a struct from LoRa
void get_struct(const void* the_structure, uint16_t size) {
	
	//the struct will be stored in the Rx_buffer, not implemented
	complete_task(1000);
}

//wait until sending is complete, or when a second has passed
void complete_task(unsigned long time_out) {
	if(_AUX != -1) {
		time_passed = 0;
		TIM_Cmd(TIM1, ENABLE);
		while(GPIO_ReadInputDataBit(GPIOA, LORA_AUX_PIN) == Bit_RESET) {
			if (time_passed > time_out) {
				break;
			}
		}
	} else {
		timer_delay(NO_AUX_DELAY);
	}
	timer_delay(MINIMUM_AFTER_AUX_DELAY);
	TIM_Cmd(TIM1, DISABLE);
}

// Zie kopje Energiezuinigheid --> LoRa modules in technisch ontwerp
void set_mode(uint8_t mode) { //setting the lora module mode
	timer_delay(PIN_RECOVER);
	switch(mode) {
		case MODE_NORMAL:
			GPIO_ResetBits(GPIOA, LORA_M0_PIN | LORA_M1_PIN);
			break;
		case MODE_WAKEUP:
			GPIO_SetBits(GPIOA, LORA_M0_PIN);
			GPIO_ResetBits(GPIOA, LORA_M1_PIN);
			break;
		case MODE_POWERDOWN:
			GPIO_ResetBits(GPIOA, LORA_M0_PIN);
			GPIO_SetBits(GPIOA, LORA_M1_PIN);
			break;
		case MODE_PROGRAM:
			GPIO_SetBits(GPIOA, LORA_M0_PIN | LORA_M1_PIN);
			break;
		default:
			break;
	}
	timer_delay(PIN_RECOVER);
	clear_buffer();
	complete_task(4000);	
}

//resetting the LoRa module. Permanent settings will not be erased
void Reset() {
	set_mode(MODE_PROGRAM);
	timer_delay(50);
	USART_putc(0xC4);
	USART_putc(0xC4);
	USART_putc(0xC4);
	complete_task(4000);
	set_mode(MODE_NORMAL);
}

// a list of setters and getters

//setting address High
void set_address_h(uint8_t val) {
	_Address_High = val;
}
uint8_t get_address_h() {
	return _Address_High;
}

//setting Address Low
void set_address_l(uint8_t val) {
	_Address_low = val;
}
uint8_t get_address_l() {
	return _Address_low;
}

//setting channel
void set_channel(uint8_t val) {
	_Channel = val;
}
uint8_t get_channel() {
	return _Channel;
}
//setting the air data rate
void set_air_data_rate(uint8_t val) {
	_Air_data_rate = val;
	build_speed_byte();
}
uint8_t get_air_data_rate() {
	return _Air_data_rate;
}

//setting whether there is a parity bit
void set_parity_bit(uint8_t val) {
	_Parity_bit = val;
	build_speed_byte();
}
uint8_t get_parity_bit( ) {
	return _Parity_bit;
}

//setting the transmission mode
void set_transmission_mode(uint8_t val) {
	_Option_transmission = val;
	build_option_byte();
}
uint8_t get_transmission_mode( ) {
	return _Option_transmission;
}

//setting the pullup mode
void set_pullup_mode(uint8_t val) {
	_Option_pullup = val;
	build_option_byte();
}
uint8_t get_pullup_mode( ) {
	return _Option_pullup;
}

//setting the WOR timing
void set_wor_timing(uint8_t val) {
	_Option_wakeup = val;
	build_option_byte();
}
uint8_t get_WOR_timing() {
	return _Option_wakeup;
}

//setting the FEC mode
void set_FEC_mode(uint8_t val) {
	_Option_FEC = val;
	build_option_byte();
}
uint8_t get_FEC_mode( ) {
	return _Option_FEC;
}

//setting the transmitting power
void set_transmit_power(uint8_t val) {

	_Option_power = val;
	build_option_byte();

}
uint8_t get_transmit_power() {
	return _Option_power;
}

//setting the address (both low and high)
void set_address(uint16_t Val) {
	_Address_High = ((Val & 0xFFFF) >> 8);
	_Address_low = (Val & 0xFF);
}
uint16_t get_address() {
	return (_Address_High << 8) | (_Address_low );
}

//setting the LoRa baudrate. Sometimes something different than 9600 will not work. 
void set_UART_baud_rate(uint8_t val) {
	_UART_data_rate = val;
	build_speed_byte();
}
uint8_t get_uart_baud_rate() {
	return _UART_data_rate;
}

/*
method to build the byte for programming (notice it's a collection of a few variables)
*/
void build_speed_byte() {
	_Speed = 0;
	_Speed = ((_Parity_bit & 0xFF) << 6) | ((_UART_data_rate & 0xFF) << 3) | (_Air_data_rate & 0xFF);
}


/*
method to build the option byte for programming (notice it's a collection of a few variables)
*/

void build_option_byte() {
	_Options = ((_Option_transmission & 0xFF) << 7) | ((_Option_pullup & 0xFF) << 6) | ((_Option_wakeup & 0xFF) << 3) | ((_Option_FEC & 0xFF) << 2) | (_Option_power & 0x03);
}

//checking whether the LoRa module is busy doing something
bool get_aux() {
	return GPIO_ReadInputDataBit(GPIOA, LORA_AUX_PIN);
}

//after using setters, settings aren't saved in the lora module yet, use this function for that.
void save_parameters(uint8_t val) {
	
	set_mode(MODE_PROGRAM);
	timer_delay(PIN_RECOVER);

	USART_putc(val);
	USART_putc(_Address_High);
	USART_putc(_Address_low);
	USART_putc(_Speed);
	USART_putc(_Channel);
	USART_putc(_Options);

	timer_delay(50);

	complete_task(4000);
	
	set_mode(MODE_NORMAL);	
}

/*
method to print parameters, this can be called anytime after init(), because init gets parameters
and any method updates the variables
*/
void print_parameters() {

	_Parity_bit = (_Speed & 0XC0) >> 6;
	_UART_data_rate = (_Speed & 0X38) >> 3;
	_Air_data_rate = _Speed & 0X07;

	_Option_transmission = (_Options & 0X80) >> 7;
	_Option_pullup = (_Options & 0X40) >> 6;
	_Option_wakeup = (_Options & 0X38) >> 3;
	_Option_FEC = (_Options & 0X07) >> 2;
	_Option_power = (_Options & 0X03);

	Serial_println("----------------------------------------");
	Serial_print("Model no.: ");  Serial_putintln(_Model);
	Serial_print("Version  : ");  Serial_putintln(_Version);
	Serial_print("Features : ");  Serial_putintln(_Features);
	Serial_println(" ");
	Serial_print("Mode : ");  Serial_putintln(_Save);
	Serial_print("AddH : ");  Serial_putintln(_Address_High);
	Serial_print("AddL : ");  Serial_putintln(_Address_low);
	Serial_print("Sped : ");  Serial_putintln(_Speed);
	Serial_print("Chan : ");  Serial_putintln(_Channel);
	Serial_print("Optn : ");  Serial_putintln(_Options);
	Serial_print("Addr : ");  Serial_putintln(get_address());
	Serial_println(" ");
	Serial_print("SpeedParityBit    : ");  Serial_putintln(_Parity_bit);
	Serial_print("SpeedUARTDataRate : ");  Serial_putintln(_UART_data_rate);
	Serial_print("SpeedAirDataRate  : ");  Serial_putintln(_Air_data_rate);
	Serial_print("OptionTrans       : ");  Serial_putintln(_Option_transmission);
	Serial_print("OptionPullup      : ");  Serial_putintln(_Option_pullup);
	Serial_print("OptionWakeup      : ");  Serial_putintln(_Option_wakeup);
	Serial_print("OptionFEC         : ");  Serial_putintln(_Option_FEC);
	Serial_print("OptionPower       : ");  Serial_putintln(_Option_power);

	Serial_println("----------------------------------------");

}

//reading parameters 
bool read_parameters() {

	_Params[0] = 0;
	_Params[1] = 0;
	_Params[2] = 0;
	_Params[3] = 0;
	_Params[4] = 0;
	_Params[5] = 0;

	set_mode(MODE_PROGRAM);

	USART_putc(0xC1);

	USART_putc(0xC1);

	USART_putc(0xC1);

	read_bytes((uint8_t*)&_Params, (uint8_t) sizeof(_Params));

	_Save = _Params[0];
	_Address_High = _Params[1];
	_Address_low = _Params[2];
	_Speed = _Params[3];
	_Channel = _Params[4];
	_Options = _Params[5];

	_Address =  (_Address_High << 8) | (_Address_low);
	_Parity_bit = (_Speed & 0XC0) >> 6;
	_UART_data_rate = (_Speed & 0X38) >> 3;
	_Air_data_rate = _Speed & 0X07;

	_Option_transmission = (_Options & 0X80) >> 7;
	_Option_pullup = (_Options & 0X40) >> 6;
	_Option_wakeup = (_Options & 0X38) >> 3;
	_Option_FEC = (_Options & 0X07) >> 2;
	_Option_power = (_Options & 0X03);
	
	set_mode(MODE_NORMAL);

	if (0xC0 != _Params[0]){
		
		return false;
	}

	return true;
	
}


//reading the model data of the module
bool read_model_data() {
	bool found;
	_Params[0] = 0;
	_Params[1] = 0;
	_Params[2] = 0;
	_Params[3] = 0;
	_Params[4] = 0;
	_Params[5] = 0;

	found = false;
	set_mode(MODE_PROGRAM);
	
	USART_putc(0xC3);
	
	USART_putc(0xC3);
	
	USART_putc(0xC3);

	
	read_bytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));
	
	
	_Save = _Params[0];	
	_Model = _Params[1];
	_Version = _Params[2];
	_Features = _Params[3];	

	if (0xC3 != _Params[0]) {
		uint8_t timeout;
		for (timeout = 0; timeout < 5; timeout++){
			#ifdef DEBUG
			Serial_print("Trying: ");Serial_putintln(timeout);
			#endif
			_Params[0] = 0;
			_Params[1] = 0;
			_Params[2] = 0;
			_Params[3] = 0;
			_Params[4] = 0;
			_Params[5] = 0;
			timer_delay(100);
			USART_putc(0xC3);
			USART_putc(0xC3);
			USART_putc(0xC3);
			read_bytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));			
			_Save = _Params[0];	
			_Model = _Params[1];
			_Version = _Params[2];
			_Features = _Params[3];	
			if (0xC3 == _Params[0]){
				found = true;
				break;
			}		
		}
	}
	else {
		found = true;
	}
	set_mode(MODE_NORMAL);
	complete_task(1000);
	return found;
	
}
/*
method to get module model and E50-TTL-100 will return 50
*/
uint8_t get_model() {
	return _Model;
}

/*
method to get module version (undocumented as to the value)
*/
uint8_t get_version() {
	return _Version;	
}

/*
method to get module version (undocumented as to the value)
*/
uint8_t get_features() {
	return _Features;
}
//a simple delay function, goes to sleeping mode while the delay isn't finished yet
void timer_delay(unsigned long delay_time) {
	time_passed = 0;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
	while(time_passed < delay_time) {
		PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
	}
	TIM_Cmd(TIM1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);
}

void read_bytes(uint8_t* buffer, uint8_t size) {
	uint8_t count = 0;
	while(count < size) {
		if(Rx_read_location == Rx_write_location && !is_full) {
			break;
		}
		buffer[count]	= Rx_buffer[Rx_read_location];
		Rx_read_location = NEXT_RX_READ_LOCATION;
		is_full = false;
	}
}
//goes to the end/beginning of the buffer, so it's empty
void clear_buffer() {
	while(Rx_read_location != Rx_write_location || is_full) {
		Rx_read_location = NEXT_RX_READ_LOCATION;
		is_full = false;
	}
}
