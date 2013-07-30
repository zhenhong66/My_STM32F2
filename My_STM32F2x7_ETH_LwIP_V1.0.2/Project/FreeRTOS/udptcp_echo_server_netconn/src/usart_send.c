#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#include "stm32f2xx_it.h"
#include "stm32_eval.h"
#include "task.h"

#define USART_SEND_THREAD_PRIO  ( tskIDLE_PRIORITY + 3 )

//extern struct netconn *conn, *newconn;
//extern u8 USART_RX_STA;
//extern u8 USART_RX_BUF[64]; 
extern uint8_t TxBuffer[0x20]; 
extern __IO uint8_t TxCounter;

uint8_t TxCmdBuffer[] = {0x01,0x02,0xE1,0x00,0x00,0x00,0x00,0xE3};

static void usart_send_thread(void *arg)
{
	 
	u8 t;
	//u8 len;	
	//err_t err;
	(void)arg;
	
	while(1)
	{			
		for(t=0;t<8;t++)
		{
			TxBuffer[t]=TxCmdBuffer[t];
			//USART3->DR=TxCmdBuffer[t];
			while((USART3->SR&0X40)==0);
		}
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		while(TxCounter < 8)
		{}
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		TxCounter=0;	
		vTaskDelay(1000);
		TxCmdBuffer[1]++;
		if( TxCmdBuffer[1] >=0x0f ) TxCmdBuffer[1]=0x02;
	}
	
}

void usart_send_init(void)
{
	sys_thread_new("usart_send_thread", usart_send_thread, NULL, DEFAULT_THREAD_STACKSIZE,USART_SEND_THREAD_PRIO);
}
#endif /* LWIP_NETCONN */
