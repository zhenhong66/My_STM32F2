#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#include "stm32f2xx_it.h"
#include "stm32_eval.h"

#define USART_TCPSEND_THREAD_PRIO  ( tskIDLE_PRIORITY + 3 )

#define cValue(x,y)	(int)(RxBuffer[(x-1)]) + (int)(RxBuffer[(y-1)])*256

extern struct netconn *conn, *newconn;
extern u8 USART_RX_STA;
extern u8 USART_RX_BUF[64];
extern uint8_t RxBuffer[68]; 
extern __IO uint16_t RxCounter;
extern uint8_t NbrOfDataToRead;

static void usart_tcpsend_thread(void *arg)
{
	
	u8 t;
	u8 len;	
	err_t err;
	(void)arg;
	
	while(1)
	{	/*		
		if(USART_RX_STA&0x80)//
		{
			if (newconn) 
      		{
				void *RxPtr;
				len=USART_RX_STA&0x3f;//
				RxPtr=USART_RX_BUF;
				//for(t=0;t<len;t++)
				//{
					//USART3->DR=USART_RX_BUF[t];
				netconn_write(newconn, RxPtr, len, NETCONN_COPY);
				while((USART3->SR&0X40)==0);//
				//}
				USART_RX_STA=0;
			}
		}
		 */
		if(USART_RX_STA&0x80)//
		{
			if (newconn) 
      		{
				void *RxPtr;
				float distance;
				float radar_30inch;
				float X;
				float short_distance;
				char str_buffer[10];
				//float long_distance=cValue(12,13); 
			
				RxPtr=RxBuffer;
				radar_30inch=cValue(34,35);
				X=cValue(9,10);
				short_distance=cValue(14,15);

				distance=30+(126.75-30)/(short_distance-radar_30inch)*(X-radar_30inch);
			    sprintf(str_buffer,"%f",distance);
			
				//printf("\n%f\n",distance);
			
				//netconn_write(newconn, RxPtr, NbrOfDataToRead, NETCONN_COPY);
				netconn_write(newconn, str_buffer, 10, NETCONN_COPY);

				while((USART3->SR&0X40)==0);//
				//}
				USART_RX_STA=0;
				
				USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
				RxCounter=0;
			}

		}

	}
}

void usart_tcpsend_init(void)
{
	sys_thread_new("usart_tcpsend_thread", usart_tcpsend_thread, NULL, DEFAULT_THREAD_STACKSIZE,USART_TCPSEND_THREAD_PRIO);
}
#endif /* LWIP_NETCONN */