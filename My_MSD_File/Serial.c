/*----------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: Low Level Serial Routines
 * Note(s): possible defines select the used communication interface:
 *            __DBG_ITM   - ITM SWO interface
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stm32f2xx.h>
#include "Serial.h"

#ifdef __DBG_ITM
volatile int32_t ITM_RxBuffer;
#endif

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void SER_Init (void) {

#ifdef __DBG_ITM
  ITM_RxBuffer   =  ITM_RXBUFFER_EMPTY;
#else
  int i;

  /* Configure UART3 for 115200 baud                                          */
  RCC->AHB1ENR  |=  (   1 <<  2);       /* Enable GPIOC clock                 */
  GPIOC->MODER  &= ~(   3 << 20);
  GPIOC->MODER  |=  (   2 << 20);       /* PC10: Alternate function mode      */
  GPIOC->AFR[1] &= ~(0x0F <<  8);
  GPIOC->AFR[1] |=  (   7 <<  8);       /* PC10: Alternate function USART3_TX */
  GPIOC->MODER  &= ~(   3 << 22);
  GPIOC->MODER  |=  (   2 << 22);       /* PC11: Alternate function mode      */
  GPIOC->AFR[1] &= ~(0x0F << 12);
  GPIOC->AFR[1] |=  (   7 << 12);       /* PC11: Alternate function USART3_RX */

  RCC->APB1ENR  |=  (   1 << 18);       /* Enable USART3 clock                */
  USART3->BRR    =  0x0100;             /* Configure 115200 baud, @ 30MHz     */
  USART3->CR3    =  0x0000;             /*           8 bit, 1 stop bit,       */
  USART3->CR2    =  0x0000;             /*           no parity                */
  for (i = 0; i < 0x1000; i++) __NOP(); /* avoid unwanted output              */
  USART3->CR1    =  0x200C;
#endif
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int SER_PutChar (int ch) {

#ifdef __DBG_ITM
  int i;
  ITM_SendChar (ch & 0xFF);
  for (i = 10000; i; i--);
#else
  while (!(USART3->SR & 0x0080));
  USART3->DR = (ch & 0xFF);
#endif
  return (ch);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (non-blocking read)
 *----------------------------------------------------------------------------*/
int SER_GetChar (void) {

#ifdef __DBG_ITM
  if (ITM_CheckChar())
    return (ITM_ReceiveChar());
#else
  if (USART3->SR & 0x0020)
    return (USART3->DR);
#endif
  return (0);
}

/*---------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
