#include "stm32f2xx_i2c.h"
#include "stm32f2xx_rcc.h"
#include "stm322xg_eval.h"
#include "stm32f2xx.h"



#define I2C_SLAVE_ADDRESS    0xA0

#ifndef I2C_SPEED
 #define I2C_SPEED                   30000    // 100000
#endif /* I2C_SPEED */




#define TSYS01_HW_ADDRESS         0xEE  


#define TSYS01_I2C_DMA_STREAM_TX            DMA1_Stream6
#define TSYS01_I2C_DMA_STREAM_RX            DMA1_Stream0  


#define TSYS01_I2C_DMA_TX_IRQHandler        DMA1_Stream6_IRQHandler
#define TSYS01_I2C_DMA_RX_IRQHandler        DMA1_Stream0_IRQHandler   


#define TSYS01_TX_DMA_FLAG_FEIF             DMA_FLAG_FEIF6
#define TSYS01_TX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF6
#define TSYS01_TX_DMA_FLAG_TEIF             DMA_FLAG_TEIF6
#define TSYS01_TX_DMA_FLAG_HTIF             DMA_FLAG_HTIF6
#define TSYS01_TX_DMA_FLAG_TCIF             DMA_FLAG_TCIF6
#define TSYS01_RX_DMA_FLAG_FEIF             DMA_FLAG_FEIF0
#define TSYS01_RX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF0
#define TSYS01_RX_DMA_FLAG_TEIF             DMA_FLAG_TEIF0
#define TSYS01_RX_DMA_FLAG_HTIF             DMA_FLAG_HTIF0
#define TSYS01_RX_DMA_FLAG_TCIF             DMA_FLAG_TCIF0


#define TSYS01_DIRECTION_TX                 0
#define TSYS01_DIRECTION_RX                 1  


#define TSYS01_I2C_DMA                      DMA1   
#define TSYS01_I2C_DMA_CHANNEL              DMA_Channel_1
#define TSYS01_I2C_DMA_STREAM_TX            DMA1_Stream6
#define TSYS01_I2C_DMA_STREAM_RX            DMA1_Stream0   
#define TSYS01_I2C_DMA_CLK                  RCC_AHB1Periph_DMA1
#define TSYS01_I2C_DR_Address               ((uint32_t)0x40005420)
#define TSYS01_USE_DMA

#define TSYS01_I2C_DMA_PREPRIO              0
#define TSYS01_I2C_DMA_SUBPRIO              0 
#define TSYS01_I2C_DMA_CLK                  RCC_AHB1Periph_DMA1


#define TSYS01_I2C                          I2C1
#define TSYS01_I2C_CLK                      RCC_APB1Periph_I2C1
#define TSYS01_I2C_SCL_PIN                  GPIO_Pin_6                  /* PB.06 */
#define TSYS01_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define TSYS01_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define TSYS01_I2C_SCL_SOURCE               GPIO_PinSource6
#define TSYS01_I2C_SCL_AF                   GPIO_AF_I2C1
#define TSYS01_I2C_SDA_PIN                  GPIO_Pin_7                  /* PB.07 */
#define TSYS01_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define TSYS01_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define TSYS01_I2C_SDA_SOURCE               GPIO_PinSource7
#define TSYS01_I2C_SDA_AF                   GPIO_AF_I2C1



#define TSYS01_MAX_TRIALS_NUMBER     300



/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define TSYS01_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define TSYS01_LONG_TIMEOUT         ((uint32_t)(10 * TSYS01_FLAG_TIMEOUT))


#define TSYS01_OK                    0
#define TSYS01_FAIL                  1   


#define TSYS01_Cmd_Reset        0x1E
#define TSYS01_Start_Temp_Conver        0x48
#define TSYS01_Read_Temp_Result        0x00
#define TSYS01_Prom_Read_Add0        0xA0
#define TSYS01_Prom_Read_Add1        0xA2
#define TSYS01_Prom_Read_Add2        0xA4
#define TSYS01_Prom_Read_Add3        0xA6
#define TSYS01_Prom_Read_Add4        0xA8
#define TSYS01_Prom_Read_Add5        0xAA
#define TSYS01_Prom_Read_Add6        0xAC
#define TSYS01_Prom_Read_Add7        0xAE



typedef unsigned char   bit;

uint32_t TSYS01_ReadBuffer(uint8_t* pBuffer , uint16_t ReadAddr, uint16_t* NumByteToRead);
uint32_t TSYS01_SetCommand(uint16_t SetCommand);
void TSYS01_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction);
uint32_t TSYS01_WaitStandbyState(void);
uint32_t TSYS01_Read2byte(uint8_t* pBuffer,uint8_t* pBuffer1, uint16_t ReadAddr, uint16_t* NumByteToRead);    
uint32_t TSYS01_Read3byte(uint8_t* pBuffer,uint8_t* pBuffer1 , uint8_t* pBuffer2 ,uint16_t ReadAddr, uint16_t* NumByteToRead);  

void TSYS01_I2C_INIT(void);
//void TSYS01_I2C_READ_PROM_WORD(char cAddress, char *cPROM);
//void TSYS01_I2C_READ_ADC(char *cADC);
//void TSYS01_I2C_START(void);
//void TSYS01_I2C_STOP(void);
//void TSYS01_I2C_TRANSMIT_BYTE(char cTransmit);
//char TSYS01_I2C_RECEIVE_BYTE(void);
//bit TSYS01_I2C_GET_ACK(void);
//void I2C_SET_ACK(bit bACK);


/* USER Callbacks: These are functions for which prototypes only are declared in
   EEPROM driver and that should be implemented into user applicaiton. */  
/* sEE_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...).
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in stm322xg_eval_i2c_ee.h file.
   Typically the user implementation of this callback should reset I2C peripheral
   and re-initialize communication or in worst case reset all the application. */
uint32_t TSYS01_TIMEOUT_UserCallback(void);




