/******************************************************************************
 * File           : Serial Eeprom 24LC512 implementation
 *****************************************************************************/
#include "se24lc512.h"
#include "EBYTE.h" //for the delay
// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
uint32_t SE_24LC512_CommStatus = SE_24LC512_COMM_OK;

// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------
void SE_WaitForI2CFlag(uint32_t flag);

/**
  * @brief  This function sets up the I2C1 hardware as mentioned in the
  *         hardware description in the header file. Afterwards I2C1 is
  *         enabled.
  * @param  None
  * @retval None
  */
void I2C_Setup(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
	init_Timer_Delay();
  // Set I2C1 clock to SYSCLK (see system_stm32f0.c)
  RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

  //(#) Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)
  //    function for I2C1 or I2C2.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  //(#) Enable SDA, SCL  and SMBA (when used) GPIO clocks using 
  //    RCC_AHBPeriphClockCmd() function. 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  //(#) Peripherals alternate function: 
  //    (++) Connect the pin to the desired peripherals' Alternate 
  //         Function (AF) using GPIO_PinAFConfig() function.
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);

  //    (++) Configure the desired pin in alternate function by:
  //         GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

  //    (++) Select the type, OpenDrain and speed via  
  //         GPIO_PuPd, GPIO_OType and GPIO_Speed members
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  //    (++) Call GPIO_Init() function.
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //(#) Program the Mode, Timing , Own address, Ack and Acknowledged Address 
  //    using the I2C_Init() function.
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
//I2C_InitStructure.I2C_Timing = 0x00310309; // ~400 kHz. @ 8 MHz (HSI) see Ref. Man. Table 91
//I2C_InitStructure.I2C_Timing = 0x50330309; // ~400 kHz. @ 48 MHz (SYSCLK) see Ref. Man. Table 93
//  I2C_InitStructure.I2C_Timing = 0x2033030A; // =400 kHz. @ 48 MHz (SYSCLK) measured with Logic Analyzer
  I2C_InitStructure.I2C_Timing = 0xB0420F13; // =100 kHz. @ 48 MHz (SYSCLK) See Table 93

  I2C_Init(I2C1, &I2C_InitStructure);
  
  //(#) Optionally you can enable/configure the following parameters without
  //    re-initialization (i.e there is no need to call again I2C_Init() function):
  //    (++) Enable the acknowledge feature using I2C_AcknowledgeConfig() function.
  //    (++) Enable the dual addressing mode using I2C_DualAddressCmd() function.
  //    (++) Enable the general call using the I2C_GeneralCallCmd() function.
  //    (++) Enable the clock stretching using I2C_StretchClockCmd() function.
  //    (++) Enable the PEC Calculation using I2C_CalculatePEC() function.
  //    (++) For SMBus Mode:
  //         (+++) Enable the SMBusAlert pin using I2C_SMBusAlertCmd() function.

  //(#) Enable the NVIC and the corresponding interrupt using the function
  //    I2C_ITConfig() if you need to use interrupt mode.
  
  //(#) When using the DMA mode 
  //   (++) Configure the DMA using DMA_Init() function.
  //   (++) Active the needed channel Request using I2C_DMACmd() function.
  
  //(#) Enable the I2C using the I2C_Cmd() function.
  I2C_Cmd(I2C1, ENABLE);
	init_Timer_Delay();
  //(#) Enable the DMA using the DMA_Cmd() function when using DMA mode in the 
  //    transfers.
}

/**
  * @brief  This function writes 8-bit data to the serial eeprom. 
  * @param  addr: 16-bit address of eeprom where data is written. 
  * @param  data: 8-bit data to be written. 
  * @retval None
  * BEWARE: Note that the eeprom needs 5 msec after a write before accepting another operation!
  */
void SE24LC512_WriteData(uint16_t addr, uint8_t data)
{
  // Wait while I2C peripheral is not ready
  SE_WaitForI2CFlag( I2C_ISR_BUSY );

  // Start I2C write transfer for 3 bytes
  I2C_TransferHandling( I2C1, SE_24LC512_ADDR, 3, I2C_AutoEnd_Mode, I2C_Generate_Start_Write );
  SE_WaitForI2CFlag( I2C_ISR_TXIS );

  // Write eeprom address
  I2C_SendData(I2C1, addr >> 8);       // Address High byte
  SE_WaitForI2CFlag(I2C_ISR_TXIS);
  I2C_SendData(I2C1, addr & 0x00FF);   // Address Low byte
  SE_WaitForI2CFlag(I2C_ISR_TXIS);

  // Write data
  I2C_SendData(I2C1, data);
    
  // Wait for- and clear stop condition
  SE_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
	timer_delay(5);
	
}


/**
  * @brief  This function clears the entire EEPROM. 
  * @param  None
  * @retval None
  */
void SE24LC512_Clear(void)
{
	uint32_t addr;
	for(addr = 0; addr < 65536; addr++) {
		SE24LC512_WriteData(addr, 0x00);
		SE_WaitForI2CFlag(I2C_ISR_BUSY);
		timer_delay(5);
	}
	SE24LC512_WriteData(0xFFFF, 0x00);
	timer_delay(5);
}



/**
  * @brief  This function reads 8-bit data from the serial eeprom 24LC512.
  * @param  addr: 16-bit address of eeprom where data is written. 
  * @retval 8-bit data read from the given address.
  */
uint8_t SE24LC512_ReadData(uint16_t addr)
{
  uint8_t data;

  // Wait while I2C peripheral is not ready
  SE_WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 2 bytes, do not end transfer (SoftEnd_Mode)
  I2C_TransferHandling(I2C1, SE_24LC512_ADDR, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  SE_WaitForI2CFlag(I2C_ISR_TXIS);

  // Write eeprom address
  I2C_SendData(I2C1, addr >> 8);       // Address High byte
  SE_WaitForI2CFlag(I2C_ISR_TXIS);
  I2C_SendData(I2C1, addr & 0x00FF);   // Address Low byte
  SE_WaitForI2CFlag(I2C_ISR_TC);

  // Repeated start I2C read transfer for 1 byte
  I2C_TransferHandling(I2C1, SE_24LC512_ADDR, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  SE_WaitForI2CFlag(I2C_ISR_RXNE);
  
  // Read data
  data = I2C_ReceiveData(I2C1);
    
  // Wait for- and clear stop condition
  SE_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C1->ICR = I2C_ICR_STOPCF;
	timer_delay(5);
  
  return( data );
}

/**
  * @brief  This function writes up to 127 bytes data to the serial eeprom. 
  * @param  addr: 16-bit address of eeprom where data is written. 
  * @param  data_ptr: pointer to data to be written. 
  * @param  lenght: number of bytes to be written (must be <= 127. 
  * @retval None
  */
void SE24LC512_WritePage(uint16_t addr, uint8_t* data_ptr, uint8_t length)
{
	//not implemented yet
}


/**
  * @brief  This function checks a flag in the I2C1 Interrupt and Status 
  *         register. The 'timeout' value indicates how many times the
  *         flag should be checked. Depending on the clock frequency of the
  *         core this takes a certain time.
  * @param  flag: the flag to check, refer to documentation of
  *         I2C_GetFlagStatus() for valid parameter values
  * @retval none
  */
void SE_WaitForI2CFlag(uint32_t flag)
{
  uint32_t timeout = SE_24LC512_TIMEOUT;
  
  if(flag == I2C_ISR_BUSY)
  {
    while(I2C_GetFlagStatus(I2C1, flag) != RESET)
    {
      if(timeout-- == 0)
      {
        SE_24LC512_CommStatus = SE_24LC512_COMM_ERROR;
        return;
      }
    }
  }
  else
  {
    while(I2C_GetFlagStatus(I2C1, flag) == RESET)
    {
      if(timeout-- == 0)
      {
        SE_24LC512_CommStatus = SE_24LC512_COMM_ERROR;
        return;
      }
    }
  }
}
