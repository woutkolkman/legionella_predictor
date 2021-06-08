#include "EBYTE.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "usart.h"
#include "stdlib.h"
#include "green_led.h"
#include "lm35.h"
#include "battery.h"
#include "adc.h"
#include "struct.h"
#include "transmitter_id.h"
#include "transmission_led.h"

//defines
// ******************************************
// when CLEARTRANSMITTERID is commented, existing transmitter ID will be used (if available)
// when CLEARTRANSMITTERID is not commented, a new transmitter ID will be generated after reset
// ******************************************
//#define CLEARTRANSMITTERID 0
