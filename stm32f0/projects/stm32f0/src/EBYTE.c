// ----------------------------------------------------------------------------
// Code for the EBYTE LoRa module.
// ----------------------------------------------------------------------------
#include <EBYTE.h>


bool LoRa_init() {
	bool ok;
	LoRa_Init_GPIO();
	
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

void LoRa_Init_GPIO() {
	GPIO_InitTypeDef GPIO_initStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE); 	//periph clock enable
	
	//GPIO for UART
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_initStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_initStructure.GPIO_Pin = LORA_USART1_PINS;
	GPIO_initStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_initStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9 | GPIO_PinSource10, GPIO_AF_1);
	
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_initStructure.GPIO_Pin = (LORA_M0_PIN | LORA_M1_PIN);
	GPIO_Init(GPIOA, &GPIO_initStructure);
	
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_initStructure.GPIO_Pin = LORA_AUX_PIN;
	GPIO_Init(GPIOA, &GPIO_initStructure);
}

void SetMode(uint8_t mode) {
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
}

bool ReadModelData() {
	_Params[0] = 0;
	_Params[1] = 0;
	_Params[2] = 0;
	_Params[3] = 0;
	_Params[4] = 0;
	_Params[5] = 0;

	bool found = false;
	int i = 0;
	
	SetMode(MODE_PROGRAM);

	_s->write(0xC3);

	_s->write(0xC3);

	_s->write(0xC3);

	_s->readBytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));
	
	_Save = _Params[0];	
	_Model = _Params[1];
	_Version = _Params[2];
	_Features = _Params[3];	

	if (0xC3 != _Params[0]) {

		for (i = 0; i < 5; i++){
			Serial.print("trying: "); Serial.println(i);
			_Params[0] = 0;
			_Params[1] = 0;
			_Params[2] = 0;
			_Params[3] = 0;
			_Params[4] = 0;
			_Params[5] = 0;
			delay(100);

			_s->write(0xC3);

			_s->write(0xC3);

			_s->write(0xC3);

			_s->readBytes((uint8_t*)& _Params, (uint8_t) sizeof(_Params));

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