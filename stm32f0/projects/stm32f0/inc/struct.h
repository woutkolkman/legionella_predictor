#ifndef _STRUCT_H_
#define _STRUCT_H_

#include "stm32f0xx.h"

#define TEMPERATURE_SIZE 		60
#define TRANSMITTER_ID_SIZE 8

// Zie LoRa communicatie --> Temperatures in technisch ontwerp
struct DATA {
	uint8_t transmitter_ID[TRANSMITTER_ID_SIZE];
	uint8_t Temperature[TEMPERATURE_SIZE];
};

extern struct DATA Temperatures;

#endif
