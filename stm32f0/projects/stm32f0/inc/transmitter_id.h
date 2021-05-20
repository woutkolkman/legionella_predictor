#include "stdint.h"
#include "struct.h"
#include "se24lc512.h"
#include "EBYTE.h" //for the delay

#define MAX_TRANSMISSION_NUMBER ('~' + 1)
#define ID_START_LOCATION 0

void make_transmitter_ID(void);
void clear_transmitter_ID(void);
void print_transmitter_ID(void);
