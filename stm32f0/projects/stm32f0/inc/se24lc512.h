/******************************************************************************
 * File           : serial eeprom 24LC512 implementation
 *****************************************************************************
   Hardware:
                24LC512
            +------ ------+                STM
            |      U      |          +--------------
      GND---+1 A0    VCC 8|----------| +3V
      GND---+2 A1     WP 7|------+   |
      GND---+3 A2    SCL 6+------|---+ I2C1_SCL/PB6
      GND-+-+4 VSS   SDA 5+------|---+ I2C1_SDA/PB7|
          | |             |      +---+ GND
          | +-------------+      |   +--------------
          +----------------------+

******************************************************************************/
#ifndef _SERIALEEPROM_H_
#define _SERIALEEPROM_H_

#include "stm32f0xx.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

//device 7-bits address assuming A0=A1=A2=0
#define SE_24LC512_ADDR               0xA0


// Maximum timeout value for waiting loops
// This timeout is not based on an accurate timing value. It just guarantess
// that the program will not get stuck in an endless loop whenever there is an
// error in the I2C communication
#define SE_24LC512_TIMEOUT            ((uint32_t)(0x00001000))

// SE_24LC512 communication status information
#define SE_24LC512_COMM_OK            (0x0)
#define SE_24LC512_COMM_ERROR         (0x1)

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void    I2C_Setup( void );
void    SE24LC512_WriteData(uint16_t addr, uint8_t data);
void    SE24LC512_Clear(void);
uint8_t SE24LC512_ReadData(uint16_t addr);
void SE24LC512_Clear_transmitter_ID(void);

// Convenience functions
void BTHQ21605V_Puts(uint8_t *str);

#endif /* _SERIALEEPROM_H_ */
