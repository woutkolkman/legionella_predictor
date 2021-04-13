/*
 * DS18B20.h
 *
 *  Created on: 13 oct. 2017
 *      Author: Farouk Majdoub
 */

#ifndef DS18B20_H_
#define DS18B20_H_

void				 		DelayDS(uint32_t);
static void					PingDS(void);
static void					InitDS(void);
static void					SendInit(void);
static void					SendDS(uint8_t  val);
static uint8_t  			ReadDS(void);
static void					ReportScratchpad(void);
static void					StartConversion(void);
static void					ReportTemperature(void);
static void					ReportROM(void);

#endif /* DS18B20_H_ */
