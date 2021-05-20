#ifndef _STRUCT_H_
#define _STRUCT_H_

#include "stm32f0xx.h"
#include "stdbool.h"

#define TEMPERATURE_SIZE 60
#define TRANSMITTER_ID_SIZE 8

struct DATA {
	uint8_t transmitter_ID[TRANSMITTER_ID_SIZE];
	uint8_t Temperature[TEMPERATURE_SIZE];
};

extern struct DATA Temperatures;

#endif
