// ----------------------------------------------------------------------------
// Code for the EBYTE LoRa module.
// ----------------------------------------------------------------------------
#include <EBYTE.h>


//global stuff

uint16_t RxReadLocation;

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
	uint8_t _AddressHigh;
	uint8_t _AddressLow;
	uint8_t _Speed;
	uint8_t _Channel;
	uint8_t _Options;

	
	// individual variables for all the options
	uint8_t _ParityBit;
	uint8_t _UARTDataRate;
	uint8_t _AirDataRate;
	uint8_t _OptionTrans;
	uint8_t _OptionPullup;
	uint8_t _OptionWakeup;
	uint8_t _OptionFEC;
	uint8_t _OptionPower;
	uint16_t _Address;
	uint8_t _Model;
	uint8_t _Version;
	uint8_t _Features;
	uint8_t _buf;

	volatile unsigned long timehad;




bool init_LoRa() {
	bool ok;
	init_LoRa_GPIO();
	init_Timer_Delay();
	init_USART();
	
	
	SetMode(MODE_NORMAL);
	
	ok = ReadModelData();
	
	if(!ok) {
		return false;
	}
	
	ok = ReadParameters();
	if(!ok) {
		return false;
	}
	
	return true;
}

void init_LoRa_GPIO() {
	GPIO_InitTypeDef GPIO_initStructure;
	
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_initStructure.GPIO_Pin = (LORA_M0_PIN | LORA_M1_PIN);
	GPIO_Init(GPIOA, &GPIO_initStructure);
	
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_initStructure.GPIO_Pin = LORA_AUX_PIN;
	GPIO_Init(GPIOA, &GPIO_initStructure);
}

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
	
	#endif
	
}

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
void init_DMA_write() {
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	// DMA channel Tx of USART Configuration
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART1->TDR);
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t) TxBuffer;
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
void init_buffer() {
	RxBuffer = (uint8_t*)malloc(sizeof(uint8_t)*RX_BUFFER_SIZE);
	if(RxBuffer == NULL) {
		//Error handling;
		HardFault_Handler();
	}
}

void init_Timer_Delay() {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Period = TIM3PERIOD;				//48000000 / 1000 / 48 = 1000 = 1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = TIM3PRESCALE;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM3_IRQn);
}

bool available() {
	return(USART1->ISR & USART_ISR_RXNE);
}

void SendByte( uint8_t TheByte) {
	USART_putc(TheByte);
	CompleteTask(1000);
}

uint8_t GetByte() {
	uint8_t result = RxBuffer[RxReadLocation];
	RxReadLocation = NEXT_RXREAD_LOCATION;
	full = false;
	CompleteTask(1000);
	return result;
}

void SendStruct(const void* TheStructure, uint16_t size) {
	uint16_t count = 0;
	while(count < size) {
		//USART_putc();
	}
	
}
void getStruct(const void* TheStructure, uint16_t size) {
	
	
}

void CompleteTask(unsigned long timeout) {
	if(_AUX != -1) {
		timehad = 0;
		TIM_Cmd(TIM3, ENABLE);
		while(GPIO_ReadInputDataBit(GPIOA, LORA_AUX_PIN) == Bit_RESET) {
			if (timehad > timeout) {
				break;
			}
		}
	} else {
		delay(NO_AUX_DELAY);
	}
	delay(MINIMUM_AFTER_AUX_DELAY);
	TIM_Cmd(TIM3, DISABLE);
}

void SetMode(uint8_t mode) {
	delay(PIN_RECOVER);
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
	delay(PIN_RECOVER);
	ClearBuffer();
	CompleteTask(4000);	
}

void Reset() {
	SetMode(MODE_PROGRAM);
	delay(50);
	USART_putc(0xC4);
	USART_putc(0xC4);
	USART_putc(0xC4);
	CompleteTask(4000);
	SetMode(MODE_NORMAL);
}

void SetAddressH(uint8_t val) {
	_AddressHigh = val;
}
uint8_t GetAddressH() {
	return _AddressHigh;
}

void SetAddressL(uint8_t val) {
	_AddressLow = val;
}
uint8_t GetAddressL() {
	return _AddressLow;
}

void SetChannel(uint8_t val) {
	_Channel = val;
}
uint8_t GetChannel() {
	return _Channel;
}

void SetAirDataRate(uint8_t val) {
	_AirDataRate = val;
	BuildSpeedByte();
}
uint8_t GetAirDataRate() {
	return _AirDataRate;
}

void SetParityBit(uint8_t val) {
	_ParityBit = val;
	BuildSpeedByte();
}
uint8_t GetParityBit( ) {
	return _ParityBit;
}

void SetTransmissionMode(uint8_t val) {
	_OptionTrans = val;
	BuildOptionByte();
}
uint8_t GetTransmissionMode( ) {
	return _OptionTrans;
}

void SetPullupMode(uint8_t val) {
	_OptionPullup = val;
	BuildOptionByte();
}
uint8_t GetPullupMode( ) {
	return _OptionPullup;
}

void SetWORTIming(uint8_t val) {
	_OptionWakeup = val;
	BuildOptionByte();
}
uint8_t GetWORTIming() {
	return _OptionWakeup;
}

void SetFECMode(uint8_t val) {
	_OptionFEC = val;
	BuildOptionByte();
}
uint8_t GetFECMode( ) {
	return _OptionFEC;
}

void SetTransmitPower(uint8_t val) {

	_OptionPower = val;
	BuildOptionByte();

}
uint8_t GetTransmitPower() {
	return _OptionPower;
}

void SetAddress(uint16_t Val) {
	_AddressHigh = ((Val & 0xFFFF) >> 8);
	_AddressLow = (Val & 0xFF);
}
uint16_t GetAddress() {
	return (_AddressHigh << 8) | (_AddressLow );
}

void SetUARTBaudRate(uint8_t val) {
	_UARTDataRate = val;
	BuildSpeedByte();
}
uint8_t GetUARTBaudRate() {
	return _UARTDataRate;
}

/*
method to build the byte for programming (notice it's a collection of a few variables)
*/
void BuildSpeedByte() {
	_Speed = 0;
	_Speed = ((_ParityBit & 0xFF) << 6) | ((_UARTDataRate & 0xFF) << 3) | (_AirDataRate & 0xFF);
}


/*
method to build the option byte for programming (notice it's a collection of a few variables)
*/

void BuildOptionByte() {
	_Options = ((_OptionTrans & 0xFF) << 7) | ((_OptionPullup & 0xFF) << 6) | ((_OptionWakeup & 0xFF) << 3) | ((_OptionFEC & 0xFF) << 2) | (_OptionPower & 0x03);
}


bool GetAux() {
	return GPIO_ReadInputDataBit(GPIOA, LORA_AUX_PIN);
}

void SaveParameters(uint8_t val) {
	
	SetMode(MODE_PROGRAM);
	
	// ClearBuffer();

	// here you can save permanenly or temp
	delay(5);

	/*
	Serial.print("val: ");
	Serial.println(val);

	Serial.print("_AddressHigh: ");
	Serial.println(_AddressHigh);

	Serial.print("_AddressLow: ");
	Serial.println(_AddressLow);

	Serial.print("_Speed: ");
	Serial.println(_Speed);

	Serial.print("_Channel: ");
	Serial.println(_Channel);

	Serial.print("_Options: ");
	Serial.println(_Options);
	*/


	USART_putc(val);
	USART_putc(_AddressHigh);
	USART_putc(_AddressLow);
	USART_putc(_Speed);
	USART_putc(_Channel);
	USART_putc(_Options);

	delay(50);

	CompleteTask(4000);
	
	SetMode(MODE_NORMAL);

	
}


void PrintParameters() {

	_ParityBit = (_Speed & 0XC0) >> 6;
	_UARTDataRate = (_Speed & 0X38) >> 3;
	_AirDataRate = _Speed & 0X07;

	_OptionTrans = (_Options & 0X80) >> 7;
	_OptionPullup = (_Options & 0X40) >> 6;
	_OptionWakeup = (_Options & 0X38) >> 3;
	_OptionFEC = (_Options & 0X07) >> 2;
	_OptionPower = (_Options & 0X03);

	Serial_println("----------------------------------------");
	Serial_print("Model no.: ");  Serial_putintln(_Model);
	Serial_print("Version  : ");  Serial_putintln(_Version);
	Serial_print("Features : ");  Serial_putintln(_Features);
	Serial_println(" ");
	Serial_print("Mode : ");  Serial_putintln(_Save);
	Serial_print("AddH : ");  Serial_putintln(_AddressHigh);
	Serial_print("AddL : ");  Serial_putintln(_AddressLow);
	Serial_print("Sped : ");  Serial_putintln(_Speed);
	Serial_print("Chan : ");  Serial_putintln(_Channel);
	Serial_print("Optn : ");  Serial_putintln(_Options);
	Serial_print("Addr : ");  Serial_putintln(GetAddress());
	Serial_println(" ");
	Serial_print("SpeedParityBit    : ");  Serial_putintln(_ParityBit);
	Serial_print("SpeedUARTDataRate : ");  Serial_putintln(_UARTDataRate);
	Serial_print("SpeedAirDataRate  : ");  Serial_putintln(_AirDataRate);
	Serial_print("OptionTrans       : ");  Serial_putintln(_OptionTrans);
	Serial_print("OptionPullup      : ");  Serial_putintln(_OptionPullup);
	Serial_print("OptionWakeup      : ");  Serial_putintln(_OptionWakeup);
	Serial_print("OptionFEC         : ");  Serial_putintln(_OptionFEC);
	Serial_print("OptionPower       : ");  Serial_putintln(_OptionPower);

	Serial_println("----------------------------------------");

}


bool ReadParameters() {

	_Params[0] = 0;
	_Params[1] = 0;
	_Params[2] = 0;
	_Params[3] = 0;
	_Params[4] = 0;
	_Params[5] = 0;

	SetMode(MODE_PROGRAM);

	USART_putc(0xC1);

	USART_putc(0xC1);

	USART_putc(0xC1);

	readBytes((uint8_t*)&_Params, (uint8_t) sizeof(_Params));

	_Save = _Params[0];
	_AddressHigh = _Params[1];
	_AddressLow = _Params[2];
	_Speed = _Params[3];
	_Channel = _Params[4];
	_Options = _Params[5];

	_Address =  (_AddressHigh << 8) | (_AddressLow);
	_ParityBit = (_Speed & 0XC0) >> 6;
	_UARTDataRate = (_Speed & 0X38) >> 3;
	_AirDataRate = _Speed & 0X07;

	_OptionTrans = (_Options & 0X80) >> 7;
	_OptionPullup = (_Options & 0X40) >> 6;
	_OptionWakeup = (_Options & 0X38) >> 3;
	_OptionFEC = (_Options & 0X07) >> 2;
	_OptionPower = (_Options & 0X03);
	
	SetMode(MODE_NORMAL);

	if (0xC0 != _Params[0]){
		
		return false;
	}

	return true;
	
}

bool ReadModelData() {
	uint8_t timeout;
	bool found;
	_Params[0] = 0;
	_Params[1] = 0;
	_Params[2] = 0;
	_Params[3] = 0;
	_Params[4] = 0;
	_Params[5] = 0;

	found = false;
	SetMode(MODE_PROGRAM);
	
	USART_putc(0xC3);
	
	USART_putc(0xC3);
	
	USART_putc(0xC3);

	
	readBytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));
	
	
	_Save = _Params[0];	
	_Model = _Params[1];
	_Version = _Params[2];
	_Features = _Params[3];	

	if (0xC3 != _Params[0]) {
		for (timeout = 0; timeout < 5; timeout++){
			Serial_print("Trying: ");Serial_putint(timeout);Serial_print("\r\n");
			_Params[0] = 0;
			_Params[1] = 0;
			_Params[2] = 0;
			_Params[3] = 0;
			_Params[4] = 0;
			_Params[5] = 0;
			delay(100);

			USART_putc(0xC3);
	
			USART_putc(0xC3);
	
			USART_putc(0xC3);

			readBytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));
			
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

	SetMode(MODE_NORMAL);

	CompleteTask(1000);

	return found;
	
}


/*
method to get module model and E50-TTL-100 will return 50
*/

uint8_t GetModel() {

	return _Model;
	
}

/*
method to get module version (undocumented as to the value)
*/

uint8_t GetVersion() {

	return _Version;
	
}

/*
method to get module version (undocumented as to the value)
*/


uint8_t GetFeatures() {

	return _Features;

}



void delay(unsigned long delayTime) {
	timehad = 0;
	TIM_Cmd(TIM3, ENABLE);
	while(timehad < delayTime) {
		//sleepysleepy
	}
	TIM_Cmd(TIM3, DISABLE);
}

void readBytes(uint8_t* buffer, uint8_t size) {
	uint8_t count = 0;
	while(count < size) {
		if(RxReadLocation == RxWriteLocation && !full) {
			break;
		}
		buffer[count]	= RxBuffer[RxReadLocation];
		RxReadLocation = NEXT_RXREAD_LOCATION;
		full = false;
	}
}

void ClearBuffer() {
	while(RxReadLocation != RxWriteLocation || full) {
		RxReadLocation = NEXT_RXREAD_LOCATION;
		full = false;
	}
}
