/*
 * Copyright (c) 2006-2013, Ari Suutari <ari@stonepile.fi>.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define NANOINTERNAL
#include <picoos.h>
#include "port_irq.h"

/*
 * Initialize STM32 usart console.
 */

#if PORTCFG_CON_USART > 0

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#define USARTx	           TOKENPASTE2(USART, PORTCFG_CON_USART)
#define USARTx_IRQHandler  TOKENPASTE2(USART, TOKENPASTE2(PORTCFG_CON_USART, _IRQHandler))
#define USARTx_IRQn        TOKENPASTE2(USART, TOKENPASTE2(PORTCFG_CON_USART, _IRQn))

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
void portInitConsole(void)
{
  USART_InitTypeDef init;

  init.USART_BaudRate = PORTCFG_CONSOLE_SPEED;
  init.USART_WordLength = USART_WordLength_8b;
  init.USART_StopBits = USART_StopBits_1;
  init.USART_Parity = USART_Parity_No;
  init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USARTx, &init);
  USART_Cmd(USARTx, ENABLE);

#if NOSCFG_FEATURE_CONIN == 1

  // Enable receive data interrupt.
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

#endif

  // Console shouldn't be realtime-critical,
  // use low interrupt priority for it.
  NVIC_SetPriority(USARTx_IRQn, PORT_CON_PRI);
  NVIC_EnableIRQ(USARTx_IRQn);
}

/*
 * Uart interrupt handler.
 */

void USARTx_IRQHandler()
{
  c_pos_intEnter();

#if NOSCFG_FEATURE_CONOUT == 1
  if (USART_GetITStatus(USARTx, USART_IT_TXE) == SET) {

    USART_ITConfig (USARTx, USART_IT_TXE, DISABLE);
    c_nos_putcharReady();
  }
#endif

#if NOSCFG_FEATURE_CONIN == 1

  unsigned char ch;

  if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET) {

    ch = USART_ReceiveData(USARTx);
    c_nos_keyinput(ch);
  }

#endif

  c_pos_intExitQuick();
}

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
  if (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
    return 0;

  USART_SendData(USARTx, c);
  USART_ITConfig (USARTx, USART_IT_TXE, ENABLE);
  return 1;
}
#endif

#endif
#endif
