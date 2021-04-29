//#define DMA 1

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_it.h"
#include "usart.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "serial.h"

// ----------------------------------------------------------------------------
// GPIO Pin Defines
// ----------------------------------------------------------------------------
#define LORA_USART1_PINS (GPIO_Pin_9 /*GPIOA9 (tx)*/| GPIO_Pin_10/*GPIOA10 (Rx)*/)
#define LORA_M0_PIN GPIO_Pin_8		//GPIOA8
#define LORA_M1_PIN GPIO_Pin_11		//GPIOA11
#define LORA_AUX_PIN GPIO_Pin_12	//GPIOA12

#define TX_BUFFER_SIZE 100 
#define RX_BUFFER_SIZE 100

//Timer 3 for delay of 1 MS
#define TIM3PERIOD (1000-1)
#define TIM3PRESCALE 48

//delay defines
#define NO_AUX_DELAY 1000
#define MINIMUM_AFTER_AUX_DELAY 20

#define NEXT_RXREAD_LOCATION ((RxReadLocation + 1) % RX_BUFFER_SIZE)

/*

if modules don't seem to save, you will have to adjust this value
when settin M0 an M1 there is gererally a short time for the transceiver modules
to react, some say only 10 ms, but I've found it can be much lonnger, I'm using
100 ms below and maybe too long, but it seemed to work in my cases

*/
#define PIN_RECOVER 15 


// modes NORMAL send and recieve for example
#define MODE_NORMAL 0			// can send and recieve
#define MODE_WAKEUP 1			// sends a preamble to waken receiver
#define MODE_POWERDOWN 2		// can't transmit but receive works only in wake up mode
#define MODE_PROGRAM 3			// for programming and deep sleep mode

// options to save change permanently or temp (power down and restart will restore settings to last saved options
#define PERMANENT 0xC0
#define TEMPORARY 0xC2

// parity bit options (must be the same for transmitter and reveiver)
#define PB_8N1 0b00			// default
#define PB_8O1 0b01
#define PB_8E1 0b11

//UART data rates
// (can be different for transmitter and reveiver)
#define UDR_1200 0b000		// 1200 baud
#define UDR_2400 0b001		// 2400 baud
#define UDR_4800 0b010		// 4800 baud
#define UDR_9600 0b011		// 9600 baud default
#define UDR_19200 0b100		// 19200 baud
#define UDR_38400 0b101		// 34800 baud
#define UDR_57600 0b110		// 57600 baud
#define UDR_115200 0b111	// 115200 baud

// air data rates (certian types of modules)
// (must be the same for transmitter and reveiver)
#define ADR_300 0b000		// 300 baud
#define ADR_1200 0b001		// 1200 baud
#define ADR_2400 0b010		// 2400 baud
#define ADR_4800 0b011		// 4800 baud
#define ADR_9600 0b100		// 9600 baud
#define ADR_19200 0b101		// 19200 baud

// air data rates (other types of modules)
#define ADR_1K 0b000		// 1k baud
#define ADR_2K 0b001		// 2K baud
#define ADR_5K 0b010		// 4K baud
#define ADR_8K 0b011		// 8K baud
#define ADR_10K 0b100		// 10K baud
#define ADR_15K 0b101		// 15K baud
#define ADR_20K 0b110		// 20K baud
#define ADR_25K 0b111		// 25K baud

// various options
// (can be different for transmitter and reveiver)
#define OPT_FMDISABLE 0b0	//default
#define OPT_FMENABLE 0b1
#define OPT_IOOPENDRAIN 0b0	 
#define OPT_IOPUSHPULL  0b1
#define OPT_WAKEUP250  0b000 
#define OPT_WAKEUP500  0b001
#define OPT_WAKEUP750  0b010
#define OPT_WAKEUP1000 0b011
#define OPT_WAKEUP1250 0b100
#define OPT_WAKEUP1500 0b101
#define OPT_WAKEUP1750 0b110
#define OPT_WAKEUP2000 0b111
#define OPT_FECDISABLE  0b0
#define OPT_FECENABLE 0b1	

// constants or 100 mW units
#define OPT_TP20 0b00		// 20 db
#define OPT_TP17 0b01		// 17 db
#define OPT_TP14 0b10		// 14 db
#define OPT_TP11 0b11		// 10 db
#define OPT_TP10 0b11		// 10 db

//init functions
	bool init_LoRa(void);
	void init_LoRa_GPIO(void);
	void init_USART(void);
	void init_USART_GPIO(void);
	void init_buffer(void);
	void init_DMA_write(void);
	void init_Timer_Delay(void);
	
	void timerDelay(unsigned long delayTime);
	
	void readBytes(uint8_t* buffer, uint8_t size);
	
	//setters
	void SetMode(uint8_t mode);
	void SetAddress(uint16_t val);
	void SetAddressH(uint8_t val);
	void SetAddressL(uint8_t val);
	void SetAirDataRate(uint8_t val);
	void SetUARTBaudRate(uint8_t val);
	void SetSpeed(uint8_t val);
	void SetOptions(uint8_t val);
	void SetChannel(uint8_t val);
	void SetParityBit(uint8_t val);
	
	//functions to set the options
	void SetTransmissionMode(uint8_t val);
	void SetPullupMode(uint8_t val);
	void SetWORTIming(uint8_t val);
	void SetFECMode(uint8_t val);
	void SetTransmitPower(uint8_t val);
	
	
	// getters
	bool GetAux(void);
	uint16_t GetAddress(void);
	uint8_t GetModel(void);
	uint8_t GetVersion(void);
	uint8_t GetFeatures(void);
	uint8_t GetAddressH(void);
	uint8_t GetAddressL(void);
	uint8_t GetAirDataRate(void);
	uint8_t GetUARTBaudRate(void);
	uint8_t GetChannel(void);
	uint8_t GetParityBit(void);
	uint8_t GetTransmissionMode(void);
	uint8_t GetPullupMode(void);
	uint8_t GetWORTIming(void);
	uint8_t GetFECMode(void);
	uint8_t GetTransmitPower(void);
	uint8_t GetOptions(void);
	uint8_t GetSpeed(void);
		
	// methods to get data from sending unit
	uint8_t GetByte(void);
	void GetStruct(const void *TheStructure, uint16_t size_);
	
	// method to send to data to receiving unit
	void SendByte(uint8_t TheByte);
	void SendStruct(const void *TheStructure, uint16_t size_);
	
	// mehod to print parameters
	void PrintParameters(void);
	
	// parameters are set above but NOT saved, here's how you save parameters
	// notion here is you can set several but save once as opposed to saving on each parameter change
	// you can save permanently (retained at start up, or temp which is ideal for dynamically changing the address or frequency
	void SaveParameters(uint8_t val);
	
	// MFG is not clear on what Reset does, but my testing indicates it clears buffer
	// I use this when needing to restart the EBYTE after programming while data is still streaming in
	// it does NOT return the ebyte back to factory defaults
	void Reset(void);

	// function to read modules parameters
	bool ReadParameters(void);

	// method to let method know of module is busy doing something (timeout provided to avoid lockups)
	void CompleteTask(unsigned long timeout);
	
	// utility funciton to build the "speed byte" which is a collection of a few different parameters
	void BuildSpeedByte(void);

	// utility funciton to build the "options byte" which is a collection of a few different parameters
	void BuildOptionByte(void);
	
	bool ReadModelData(void);
	void ClearBuffer(void);
	bool available(void);
	
	//global used variables, from other files, needed here.
	extern volatile uint8_t *RxBuffer;
	extern uint16_t RxWriteLocation;
	
	
	extern bool full;