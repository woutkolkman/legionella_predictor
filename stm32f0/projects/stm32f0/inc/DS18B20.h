#ifndef DS18B20_H_
#define DS18B20_H_

#include "stdint.h"

// function prototypes
//void 				 delay_DS(const int d);
void 					 delay_DS(uint32_t usecs);
static void    ping_DS(void);
void           init_sensor(void);
static void	   send_init(void);
static void	   send_DS(uint8_t value);
static uint8_t read_DS(void);
static void		 report_scratchpad(void);
static void		 start_conversion(void);
static void		 report_temperature(void);
static void		 report_ROM(void);
float          check_DS(void);
//void				 check_DS(void);

#endif 
