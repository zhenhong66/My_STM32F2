
#include "i2c_TSYS01.h"
#include "stm32f2xx_dma.h"





DMA_InitTypeDef    TSYS01_DMA_InitStructure; 
extern NVIC_InitTypeDef   NVIC_InitStructure;

__IO uint16_t  TSYS01_Address = 0; 
__IO uint32_t  TSYS01_Timeout = TSYS01_LONG_TIMEOUT;  
__IO uint16_t* TSYS01_DataReadPointer;   

/******************************************************************
	* Function: TSYS01_I2C_INIT *
	* Input: --- *
	* Return: --- *
	* Description: Initialization of I2C Port *
*******************************************************************/
	void TSYS01_I2C_INIT(void)
	{

		
		I2C_InitTypeDef  I2C_InitStructure;
		GPIO_InitTypeDef  GPIO_InitStructure; 

		/*!< TSYS01_I2C Periph clock enable */
  		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

		/*!< TSYS01_I2C_SCL_GPIO_CLK and TSYS01_I2C_SDA_GPIO_CLK Periph clock enable */
  		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		/* Reset sEE_I2C IP */
  		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  		/* Release reset signal of sEE_I2C IP */
  		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

		/*!< GPIO configuration */  
		  /*!< Configure sEE_I2C pins: SCL */   
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//GPIO_Speed_50MHz;
		  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	;//   GPIO_PuPd_UP
		  GPIO_Init(GPIOB, &GPIO_InitStructure);

		/*!< Configure TSYS01_I2C pins: SDA */
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		  GPIO_Init(GPIOB, &GPIO_InitStructure);

		 /* Connect PXx to I2C_SCL*/
  		  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);

		  /* Connect PXx to I2C_SDA*/
		  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);  

		 


		  /* Configure and enable I2C DMA TX Channel interrupt */
		  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
		  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TSYS01_I2C_DMA_PREPRIO ;
		  NVIC_InitStructure.NVIC_IRQChannelSubPriority = TSYS01_I2C_DMA_SUBPRIO;
		  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		  NVIC_Init(&NVIC_InitStructure);
		
		  /* Configure and enable I2C DMA RX Channel interrupt */
		  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
		  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TSYS01_I2C_DMA_PREPRIO;
		  NVIC_InitStructure.NVIC_IRQChannelSubPriority = TSYS01_I2C_DMA_SUBPRIO;

		  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		  NVIC_Init(&NVIC_InitStructure);  
		  
		  /*!< I2C DMA TX and RX channels configuration */
		  /* Enable the DMA clock */
		  RCC_AHB1PeriphClockCmd(TSYS01_I2C_DMA_CLK, ENABLE);
		  
		  /* Clear any pending flag on Rx Stream  */
		  DMA_ClearFlag(TSYS01_I2C_DMA_STREAM_TX, TSYS01_TX_DMA_FLAG_FEIF | TSYS01_TX_DMA_FLAG_DMEIF | TSYS01_TX_DMA_FLAG_TEIF | \
		                                       TSYS01_TX_DMA_FLAG_HTIF | TSYS01_TX_DMA_FLAG_TCIF);
		  /* Disable the EE I2C Tx DMA stream */
		  DMA_Cmd(TSYS01_I2C_DMA_STREAM_TX, DISABLE);
		  /* Configure the DMA stream for the EE I2C peripheral TX direction */
		  DMA_DeInit(TSYS01_I2C_DMA_STREAM_TX);
		  TSYS01_DMA_InitStructure.DMA_Channel = TSYS01_I2C_DMA_CHANNEL;
		  TSYS01_DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TSYS01_I2C_DR_Address;
		  TSYS01_DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;    /* This parameter will be configured durig communication */;
		  TSYS01_DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; /* This parameter will be configured durig communication */
		  TSYS01_DMA_InitStructure.DMA_BufferSize = 0xFFFF;              /* This parameter will be configured durig communication */
		  TSYS01_DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		  TSYS01_DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		  TSYS01_DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		  TSYS01_DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		  TSYS01_DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		  TSYS01_DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
		  TSYS01_DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
		  TSYS01_DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
		  TSYS01_DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		  TSYS01_DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		  DMA_Init(TSYS01_I2C_DMA_STREAM_TX, &TSYS01_DMA_InitStructure);
		
		  /* Clear any pending flag on Rx Stream */
		  DMA_ClearFlag(TSYS01_I2C_DMA_STREAM_RX, TSYS01_RX_DMA_FLAG_FEIF | TSYS01_RX_DMA_FLAG_DMEIF | TSYS01_RX_DMA_FLAG_TEIF | \
		                                       TSYS01_RX_DMA_FLAG_HTIF | TSYS01_RX_DMA_FLAG_TCIF);
		  /* Disable the EE I2C DMA Rx stream */
		  DMA_Cmd(TSYS01_I2C_DMA_STREAM_RX, DISABLE);
		  /* Configure the DMA stream for the EE I2C peripheral RX direction */
		  DMA_DeInit(TSYS01_I2C_DMA_STREAM_RX);
		  while (DMA_GetCmdStatus(TSYS01_I2C_DMA_STREAM_RX) != DISABLE)
		  {
		  }

		  DMA_Init(TSYS01_I2C_DMA_STREAM_RX, &TSYS01_DMA_InitStructure);
		  
		  /* Enable the DMA Channels Interrupts */
		  DMA_ITConfig(TSYS01_I2C_DMA_STREAM_TX, DMA_IT_TC, ENABLE);
		  DMA_ITConfig(TSYS01_I2C_DMA_STREAM_RX, DMA_IT_TC, ENABLE);  


		 I2C_DeInit(I2C1);
		 /*!< I2C configuration */
		  /* sEE_I2C configuration */
		  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
		  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
		  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS;
		  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;


		  /* sEE_I2C Peripheral Enable */
		  I2C_Cmd(I2C1, ENABLE);
		  /* Apply sEE_I2C configuration after enabling it */
		  I2C_Init(I2C1, &I2C_InitStructure);

		
		  TSYS01_Address = TSYS01_HW_ADDRESS; 
		//I2C_SCK_DIR = OUT; // SCK = Output
		//I2C_SDA_DIR = OUT; // SDA = Output
	}


uint32_t TSYS01_Read2byte(uint8_t* pBuffer,uint8_t* pBuffer1 ,uint16_t ReadAddr, uint16_t* NumByteToRead)
{  
  	TSYS01_DataReadPointer = NumByteToRead;
  
  /*!< While the bus is busy */
  TSYS01_Timeout = TSYS01_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BUSY))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
  
  /*!< Send START condition */
  I2C_GenerateSTART(TSYS01_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
  
  /*!< Send EEPROM address for write */
  I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  } 


  
  /*!< Send the EEPROM's internal address to read from: Only one byte address */
  I2C_SendData(TSYS01_I2C, ReadAddr);  


  /*!< Test on EV8 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }

  I2C_GenerateSTOP(TSYS01_I2C, ENABLE); //
  
  /*!< Send STRAT condition a second time */  
  I2C_GenerateSTART(TSYS01_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  } 
  
  /*!< Send EEPROM address for read */
  I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Receiver);  
  
  /* If number of data to be read is 1, then DMA couldn't be used */
  /* One Byte Master Reception procedure (POLLING) ---------------------------*/

    /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_ADDR) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }     
    

    
    /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
    (void)TSYS01_I2C->SR2;
    

    
    /* Wait for the byte to be received */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_RXNE) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }
    
    /*!< Read the byte received from the EEPROM */
    *pBuffer = I2C_ReceiveData(TSYS01_I2C);
	
    
    /*!< Decrement the read bytes counter */
    (uint16_t)(*NumByteToRead)--;        



    
    /* Wait for the byte to be received */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_RXNE) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }
    
    /*!< Read the byte received from the EEPROM */
    *pBuffer1 = I2C_ReceiveData(TSYS01_I2C);
	
    
    /*!< Decrement the read bytes counter */
    (uint16_t)(*NumByteToRead)--;       
	
	
	/*!< Disable Acknowledgment */
    I2C_AcknowledgeConfig(TSYS01_I2C, DISABLE);    


	/*!< Send STOP Condition */		//test
    I2C_GenerateSTOP(TSYS01_I2C, ENABLE);

    
    /* Wait to make sure that STOP control bit has been cleared */
	
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(TSYS01_I2C->CR1 & I2C_CR1_STOP)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    } 
	 
    
    /*!< Re-Enable Acknowledgment to be ready for another reception */
    I2C_AcknowledgeConfig(TSYS01_I2C, ENABLE);       
  
  
  /* If all operations OK, return sEE_OK (0) */
  return TSYS01_OK;
}



uint32_t TSYS01_Read3byte(uint8_t* pBuffer,uint8_t* pBuffer1 , uint8_t* pBuffer2 ,uint16_t ReadAddr, uint16_t* NumByteToRead)
{  
  	TSYS01_DataReadPointer = NumByteToRead;
  
  /*!< While the bus is busy */
  TSYS01_Timeout = TSYS01_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BUSY))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
  
  /*!< Send START condition */
  I2C_GenerateSTART(TSYS01_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
  
  /*!< Send EEPROM address for write */
  I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  } 


  
  /*!< Send the EEPROM's internal address to read from: Only one byte address */
  I2C_SendData(TSYS01_I2C, ReadAddr);  


  /*!< Test on EV8 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }

  I2C_GenerateSTOP(TSYS01_I2C, ENABLE); //
  
  /*!< Send STRAT condition a second time */  
  I2C_GenerateSTART(TSYS01_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  } 
  
  /*!< Send EEPROM address for read */
  I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Receiver);  
  
  /* If number of data to be read is 1, then DMA couldn't be used */
  /* One Byte Master Reception procedure (POLLING) ---------------------------*/

    /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_ADDR) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }     
    

    
    /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
    (void)TSYS01_I2C->SR2;
    

    
    /* Wait for the byte to be received */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_RXNE) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }
    
    /*!< Read the byte received from the EEPROM */
    *pBuffer = I2C_ReceiveData(TSYS01_I2C);
	
    
    /*!< Decrement the read bytes counter */
    (uint16_t)(*NumByteToRead)--;        



    
    /* Wait for the byte to be received */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_RXNE) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }
    
    /*!< Read the byte received from the EEPROM */
    *pBuffer1 = I2C_ReceiveData(TSYS01_I2C);
	
    
    /*!< Decrement the read bytes counter */
    (uint16_t)(*NumByteToRead)--;     
	
	
	
	/* Wait for the byte to be received */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_RXNE) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }
    
    /*!< Read the byte received from the EEPROM */
    *pBuffer2 = I2C_ReceiveData(TSYS01_I2C);
	
    
    /*!< Decrement the read bytes counter */
    (uint16_t)(*NumByteToRead)--;     
	
	
	/*!< Disable Acknowledgment */
    I2C_AcknowledgeConfig(TSYS01_I2C, DISABLE);    


	/*!< Send STOP Condition */		//test
    I2C_GenerateSTOP(TSYS01_I2C, ENABLE);

    
    /* Wait to make sure that STOP control bit has been cleared */
	
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(TSYS01_I2C->CR1 & I2C_CR1_STOP)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    } 
	 
    
    /*!< Re-Enable Acknowledgment to be ready for another reception */
    I2C_AcknowledgeConfig(TSYS01_I2C, ENABLE);       
  
  
  /* If all operations OK, return sEE_OK (0) */
  return TSYS01_OK;
}


uint32_t TSYS01_ReadBuffer(uint8_t* pBuffer ,uint16_t ReadAddr, uint16_t* NumByteToRead)
{  
  /* Set the pointer to the Number of data to be read. This pointer will be used 
      by the DMA Transfer Completer interrupt Handler in order to reset the 
      variable to 0. User should check on this variable in order to know if the 
      DMA transfer has been complete or not. */
  TSYS01_DataReadPointer = NumByteToRead;
  
  /*!< While the bus is busy */
  TSYS01_Timeout = TSYS01_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BUSY))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
  
  /*!< Send START condition */
  I2C_GenerateSTART(TSYS01_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
  
  /*!< Send EEPROM address for write */
  I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  } 


  
  /*!< Send the EEPROM's internal address to read from: Only one byte address */
  I2C_SendData(TSYS01_I2C, ReadAddr);  


  /*!< Test on EV8 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }

  I2C_GenerateSTOP(TSYS01_I2C, ENABLE); //
  
  /*!< Send STRAT condition a second time */  
  I2C_GenerateSTART(TSYS01_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  } 
  
  /*!< Send EEPROM address for read */
  I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Receiver);  
  
  /* If number of data to be read is 1, then DMA couldn't be used */
  /* One Byte Master Reception procedure (POLLING) ---------------------------*/
  if ((uint16_t)(*NumByteToRead) < 2)
  {
    /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_ADDR) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }     
    
    /*!< Disable Acknowledgment */
    I2C_AcknowledgeConfig(TSYS01_I2C, DISABLE);   
    
    /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
    (void)TSYS01_I2C->SR2;
    
    /*!< Send STOP Condition */
    //I2C_GenerateSTOP(TSYS01_I2C, ENABLE);		  //
    
    /* Wait for the byte to be received */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_RXNE) == RESET)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }
    
    /*!< Read the byte received from the EEPROM */
    *pBuffer = I2C_ReceiveData(TSYS01_I2C);
	
    
    /*!< Decrement the read bytes counter */
    (uint16_t)(*NumByteToRead)--;        


	/*!< Send STOP Condition */		//test
    I2C_GenerateSTOP(TSYS01_I2C, ENABLE);

    
    /* Wait to make sure that STOP control bit has been cleared */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(TSYS01_I2C->CR1 & I2C_CR1_STOP)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }  
    
    /*!< Re-Enable Acknowledgment to be ready for another reception */
    I2C_AcknowledgeConfig(TSYS01_I2C, ENABLE);    
  }
  else/* More than one Byte Master Reception procedure (DMA) -----------------*/
  {
    /*!< Test on EV6 and clear it */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }  
    
    /* Configure the DMA Rx Channel with the buffer address and the buffer size */
    TSYS01_LowLevel_DMAConfig((uint32_t)pBuffer, (uint16_t)(*NumByteToRead), TSYS01_DIRECTION_RX);
    
    /* Inform the DMA that the next End Of Transfer Signal will be the last one */
    I2C_DMALastTransferCmd(TSYS01_I2C, ENABLE); 
    
    /* Enable the DMA Rx Stream */
    DMA_Cmd(TSYS01_I2C_DMA_STREAM_RX, ENABLE);    
  }
  
  /* If all operations OK, return sEE_OK (0) */
  return TSYS01_OK;
}



uint32_t TSYS01_SetCommand(uint16_t SetCommand)
{  
 
  
  /*!< While the bus is busy */
  TSYS01_Timeout = TSYS01_LONG_TIMEOUT;
  
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BUSY))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
   
  /*!< Send START condition */
  I2C_GenerateSTART(TSYS01_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
  
  /*!< Send TSYS01 address for write */
  I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  } 


  
  /*!< Send the EEPROM's internal address to read from: Only one byte address */
  I2C_SendData(TSYS01_I2C, SetCommand);  


  /*!< Test on EV8 and clear it */
  TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }
		 /*
		 TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
	    while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_ADDR) == RESET)
	    {
	      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
	    }     
    	*/
    /*!< Disable Acknowledgment */
    I2C_AcknowledgeConfig(TSYS01_I2C, DISABLE);   
    
    /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
    (void)TSYS01_I2C->SR2;
    
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(TSYS01_I2C, ENABLE);



	/* Wait to make sure that STOP control bit has been cleared */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(TSYS01_I2C->CR1 & I2C_CR1_STOP)
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }  
    
    /*!< Re-Enable Acknowledgment to be ready for another reception */
    I2C_AcknowledgeConfig(TSYS01_I2C, ENABLE); 
  
  
  /* If all operations OK, return sEE_OK (0) */
  return TSYS01_OK;
}




void TSYS01_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction)
{ 
  /* Initialize the DMA with the new parameters */
  if (Direction == TSYS01_DIRECTION_TX)
  {
    /* Configure the DMA Tx Stream with the buffer address and the buffer size */
    TSYS01_DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pBuffer;
    TSYS01_DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;    
    TSYS01_DMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;  
    DMA_Init(TSYS01_I2C_DMA_STREAM_TX, &TSYS01_DMA_InitStructure);  
  }
  else
  { 
    /* Configure the DMA Rx Stream with the buffer address and the buffer size */
    TSYS01_DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pBuffer;
    TSYS01_DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    TSYS01_DMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;      
    DMA_Init(TSYS01_I2C_DMA_STREAM_RX, &TSYS01_DMA_InitStructure);    
  }
}



uint32_t TSYS01_WaitStandbyState(void)      
{
  __IO uint16_t tmpSR1 = 0;
  __IO uint32_t TSYS01_Trials = 0;

  /*!< While the bus is busy */
  TSYS01_Timeout = TSYS01_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BUSY))
  {
    if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
  }

  /* Keep looping till the slave acknowledge his address or maximum number 
     of trials is reached (this number is defined by sEE_MAX_TRIALS_NUMBER define
     in stm322xg_eval_i2c_ee.h file) */
  while (1)
  {
    /*!< Send START condition */
    I2C_GenerateSTART(TSYS01_I2C, ENABLE);

    /*!< Test on EV5 and clear it */
    TSYS01_Timeout = TSYS01_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(TSYS01_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }    

    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(TSYS01_I2C, TSYS01_Address, I2C_Direction_Transmitter);
    
    /* Wait for ADDR flag to be set (Slave acknowledged his address) */
    TSYS01_Timeout = TSYS01_LONG_TIMEOUT;
    do
    {     
      /* Get the current value of the SR1 register */
      tmpSR1 = TSYS01_I2C->SR1;
      
      /* Update the timeout value and exit if it reach 0 */
      if((TSYS01_Timeout--) == 0) return TSYS01_TIMEOUT_UserCallback();
    }
    /* Keep looping till the Address is acknowledged or the AF flag is 
       set (address not acknowledged at time) */
    while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);
     
    /* Check if the ADDR flag has been set */
    if (tmpSR1 & I2C_SR1_ADDR)
    {
      /* Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already 
         been read) */
      (void)TSYS01_I2C->SR2;
      
      /*!< STOP condition */    
      I2C_GenerateSTOP(TSYS01_I2C, ENABLE);
        
      /* Exit the function */
      return TSYS01_OK;
    }
    else
    {
      /*!< Clear AF flag */
      I2C_ClearFlag(TSYS01_I2C, I2C_FLAG_AF);                  
    }
    
    /* Check if the maximum allowed number of trials has bee reached */
    if (TSYS01_Trials++ == TSYS01_MAX_TRIALS_NUMBER)
    {
      /* If the maximum number of trials has been reached, exit the function */
      return TSYS01_TIMEOUT_UserCallback();
    }
  }
}




/**
  * @brief  This function handles the DMA Tx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void TSYS01_I2C_DMA_TX_IRQHandler(void)
{
  /* Check if the DMA transfer is complete */
  if(DMA_GetFlagStatus(TSYS01_I2C_DMA_STREAM_TX, TSYS01_TX_DMA_FLAG_TCIF) != RESET)
  {  
    /* Disable the DMA Tx Stream and Clear TC flag */  
    DMA_Cmd(TSYS01_I2C_DMA_STREAM_TX, DISABLE);
    DMA_ClearFlag(TSYS01_I2C_DMA_STREAM_TX, TSYS01_TX_DMA_FLAG_TCIF);

    /*!< Wait till all data have been physically transferred on the bus */
    TSYS01_Timeout = TSYS01_LONG_TIMEOUT;
    while(!I2C_GetFlagStatus(TSYS01_I2C, I2C_FLAG_BTF))
    {
      if((TSYS01_Timeout--) == 0) TSYS01_TIMEOUT_UserCallback();
    }
    
    /*!< Send STOP condition */
    I2C_GenerateSTOP(TSYS01_I2C, ENABLE);
    
    /* Reset the variable holding the number of data to be written */
    //*TSYS01_DataWritePointer = 0;  
  }
}

/**
  * @brief  This function handles the DMA Rx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void TSYS01_I2C_DMA_RX_IRQHandler(void)
{
  /* Check if the DMA transfer is complete */
  if(DMA_GetFlagStatus(TSYS01_I2C_DMA_STREAM_RX, TSYS01_RX_DMA_FLAG_TCIF) != RESET)
  {      
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(TSYS01_I2C, ENABLE);    
    
    /* Disable the DMA Rx Stream and Clear TC Flag */  
    DMA_Cmd(TSYS01_I2C_DMA_STREAM_RX, DISABLE);
    DMA_ClearFlag(TSYS01_I2C_DMA_STREAM_RX, TSYS01_RX_DMA_FLAG_TCIF);
    
    /* Reset the variable holding the number of data to be read */
    *TSYS01_DataReadPointer = 0;
  }
}



/******************************************************************
	* Function: TSYS01_I2C_READ_PROM_WORD *
	* Input: char cAddress Address of Prom to be read *
	** Return: cPPROM[2] via call by reference *
	* Description: Reads two byte (one word) of Prom memory *
*******************************************************************/
/*
	void TSYS01_I2C_READ_PROM_WORD(char cAddress, char *cPROM)
	{
		cAddress = 0xA0 | (cAddress << 1);
		TSYS01_I2C_START(); // Send Start Condition
		TSYS01_I2C_TRANSMIT_BYTE(I2C_ADRESS | I2C_W); // Send I2C-Address, Write // Mode
		TSYS01_I2C_GET_ACK(); // Get ACK
		TSYS01_I2C_SEND_BYTE(cAddress); // Send Read PROM command // including address to read
		TSYS01_I2C_GET_ACK(); // Get ACK
		TSYS01_I2C_STOP(); // Send Stop Condition
		TSYS01_I2C_START(); // Send Start Condition
		TSYS01_I2C_TRANSMIT_BYTE(I2C_ADRESS | I2C_R); // Send I2C-Address, Read Mode
		TSYS01_I2C_GET_ACK(); // Get ACK
		cPPROM[0] = TSYS01_I2C_RECEIVE_BYTE(); // Read high byte
		I2C_SET_ACK(TRUE); // Set ACK
		cPPROM[1] = TSYS01_I2C_RECEIVE_BYTE(); // Read low byte
		I2C_SET_ACK(FALSE); // Set ACK
		TSYS01_I2C_STOP(); // Send Stop Condition
	}
*/
/******************************************************************
* Function: TSYS01_I2C_READ_ADC *
* Input: --- *
* Return: cADC[4] via call by reference *
* Description: Reads four bytes of ADC result (24bit) *
*******************************************************************/
/*
	void TSYS01_I2C_READ_ADC(char *cADC)
	{
		char cByte;
		// Send command to start ADC conversion
		TSYS01_I2C_START(); // Send Start Condition
		TSYS01_I2C_TRANSMIT_BYTE(I2C_ADRESS | I2C_W); // Send I2C-Address, Write // Mode
		TSYS01_I2C_GET_ACK(); // Get ACK
		TSYS01_I2C_SEND_BYTE(0x48); // Start Conversion
		TSYS01_I2C_GET_ACK(); // Get ACK
		TSYS01_I2C_STOP(); // Send Stop Condition
		// Repeat this block until Acknowledge is true
		// or wait 10ms for conversion to be done
		TSYS01_I2C_START(); // Send Start Condition
		TSYS01_I2C_TRANSMIT_BYTE(I2C_ADRESS | I2C_W); // Send I2C-Address, Write Mode
		TSYS01_I2C_GET_ACK(); // Get ACK
		TSYS01_I2C_STOP(); // Send Stop Condition
		TSYS01_I2C_START(); // Send Start Condition
		TSYS01_I2C_TRANSMIT_BYTE(I2C_ADRESS | I2C_W); // Send I2C-Address, Write Mode
		TSYS01_I2C_GET_ACK(); // Get ACK
		TSYS01_I2C_SEND_BYTE(0x00); // Send Read ADC command
		TSYS01_I2C_GET_ACK(); // Get ACK
		TSYS01_I2C_STOP(); // Send Stop Condition
		TSYS01_I2C_START(); // Send Start Condition
		TSYS01_I2C_TRANSMIT_BYTE(I2C_ADRESS | I2C_R); // Send I2C-Address, Read Mode
		TSYS01_I2C_GET_ACK(); // Get ACK
		cADC[0] = TSYS01_I2C_RECEIVE_BYTE(); // Read first byte
		I2C_SET_ACK(TRUE); // Set ACK
		cADC[1] = TSYS01_I2C_RECEIVE_BYTE(); // Read next byte
		I2C_SET_ACK(TRUE); // Set ACK
		cADC[2] = TSYS01_I2C_RECEIVE_BYTE(); // Read next byte
		I2C_SET_ACK(TRUE); // Set ACK
		cADC[3] = TSYS01_I2C_RECEIVE_BYTE(); // Read last byte
		I2C_SET_ACK(FALSE); // Set NACK
		TSYS01_I2C_STOP(); // Send Stop Condition
	}

 */
/******************************************************************
	* Function: TSYS01_I2C_START *
	* Input: --- *
	* Return: --- *
	* Description: Send I2C Start Condition *
*******************************************************************/
/*
	void TSYS01_I2C_START(void)
	{
	
		I2C_SCK_DIR = OUT; // SCK = Output
		I2C_SDA_DIR = OUT; // SDA = Output
		I2C_SCK = 1;
		I2C_SDA = 1;
		I2C_SDA = 0;
	

	}
	*/
/******************************************************************
* Function: TSYS01_I2C_STOP *
* Input: --- *
* Return: --- *
* Description: Send I2C Stop Condition *
*******************************************************************/
/*
	void TSYS01_I2C_STOP(void)
	{
		
		I2C_SCK_DIR = OUT; // SCK is Output
		I2C_SDA_DIR = OUT; // SDA is Output
		I2C_SCK = 1;
		I2C_SDA = 0;
		I2C_SDA = 1;
	
	 
	}
*/
/******************************************************************
	* Function: TSYS01_I2C_TRANSMIT_BYTE *
	* Input: char cTransmit Byte to be send to TSYS01 *
	* Return: --- *
	* Description: Sends one byte to TSYS01 *
*******************************************************************/
/*
	void TSYS01_I2C_TRANSMIT_BYTE(char cTransmit)
	{
		char cBit, cMask;
		cMask = 0x80;
		I2C_SCK_DIR = OUT; // SCK is Output
		I2C_SDA_DIR = OUT; // SDA is Output
		I2C_SCK = 0;
		for (cBit = 0; cBit < 8; cBit ++)
		{
			I2C_SDA = 0;
			if ((cTransmit & cMask) != 0) I2C_SDA = 1;
			I2C_SCK = 1;
			I2C_SCK = 0;
			cMask = cMask >> 1;
		}
	}
 */

/******************************************************************
	Function: TSYS01_I2C_RECEIVE_BYTE
	Input: ---
	Return: char cReceive Byte received from TSYS01
	Description: Reads one byte from TSYS01
*******************************************************************/
/*
	char TSYS01_I2C_RECEIVE_BYTE(void)
	{
		char cReceive, cBit;
		I2C_SCK_DIR = IN; // SCK is Input
		I2C_SDA_DIR = IN; // SDA is Input
		while (I2C_SCK == 0); // Wait for SCL release
		I2C_SCK_DIR = OUT; // SCK is Output
		I2C_SCK = 0;
		I2C_SCK = 1;
		for (cBit = 0; cBit < 8; cBit++)
		{
			cReceive = cReceive << 1;
			I2C_SCK = 1;
			if (I2C_SDA == 1) cReceive = cReceive + 1;
			I2C_SCK = 0;
		}
		return cReceive;
	}
  */
/******************************************************************
	* Function: TSYS01_I2C_GET_ACK *
	* Input: --- *
	* Return: bit bACK Bit represents ACK status *
	* Description: Reads Acknowledge from TSYS01 *
*******************************************************************/
/*
	bit TSYS01_I2C_GET_ACK(void)
	{
		bit bACK;
		I2C_SCK_DIR = OUT; // SCK is Output
		I2C_SDA_DIR = IN; // SDA is Input
		I2C_SCK = 0;
		I2C_SCK = 1;
		bACK = I2C_SDA;
		I2C_SCK = 0;
		return bACK;
	}
*/
/******************************************************************
	Function: TSYS01_I2C_Set_ACK
	Input: bit bACK Bit represents ACK status to be send
	Return: ---
	Description: Reads Acknowledge from TSYS01
*******************************************************************/
/*
	void I2C_SET_ACK(bit bACK)
	{
		I2C_SCK_DIR = OUT; // SCK is Output
		I2C_SDA_DIR = OUT; // SDA is Output
		I2C_SCK = 0;
		I2C_SDA = bACK;
		I2C_SCK = 1;
		I2C_SCK = 0;
	}

 */
