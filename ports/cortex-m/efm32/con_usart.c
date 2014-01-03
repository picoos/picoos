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

#include "em_usart.h"
#include "em_cmu.h"

/*
 * Initialize Energy Micro usart console.
 */

#if PORTCFG_CON_USART == 1

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

static USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

void portInitConsole(void)
{
  CMU_ClockEnable(cmuClock_USART1, true);

  // Initialize uart to correct speed, 8 bits, no parity.

  uartInit.enable       = usartDisable;
  uartInit.refFreq      = 0;
  uartInit.baudrate     = PORTCFG_CONSOLE_SPEED;
  uartInit.oversampling = usartOVS16;
  uartInit.databits     = usartDatabits8;
  uartInit.parity       = usartNoParity;
  uartInit.stopbits     = usartStopbits1;

  USART_InitAsync(USART1, &uartInit);

  USART_IntClear(USART1, _USART_IF_MASK);
  USART_IntEnable(USART1, USART_IF_RXDATAV);

  NVIC_ClearPendingIRQ(USART1_TX_IRQn);

  // Console shouldn't be realtime-critical,
  // use low interrupt priority for it.
  NVIC_SetPriority(USART1_TX_IRQn, PORT_PENDSV_PRI - 1);
  NVIC_EnableIRQ(USART1_TX_IRQn);

#if NOSCFG_FEATURE_CONIN == 1

  NVIC_ClearPendingIRQ(USART1_RX_IRQn);
  NVIC_SetPriority(USART1_RX_IRQn, PORT_PENDSV_PRI - 1);
  NVIC_EnableIRQ(USART1_RX_IRQn);

  #endif

  USART1->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC0;

  USART_Enable(USART1, usartEnable);
}

/*
 * Usart TX interrupt handler.
 */

void USART1_TX_IRQHandler()
{
  c_pos_intEnter();

  if (USART1->STATUS & USART_STATUS_TXBL) {

    USART_IntDisable(USART1, USART_IF_TXBL);
    c_nos_putcharReady();
  }

  c_pos_intExitQuick();
}

#if NOSCFG_FEATURE_CONIN == 1

/*
 * Usart RX interrupt handler.
 */

void USART1_RX_IRQHandler()
{
  c_pos_intEnter();

  if (USART1->STATUS & USART_STATUS_RXDATAV) {

    unsigned char ch;

    ch = USART_Rx(uart);
    USART_IntClear(USART1, USART_IF_RXDATAV);

    c_nos_keyinput(ch);
  }

  c_pos_intExitQuick();
}

#endif

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
  if (!(USART1->STATUS & USART_STATUS_TXBL))
    return 0;

  USART_Tx(USART1, c);
  USART_IntEnable(USART1, USART_IF_TXBL);
  return 1;
}
#endif

#endif
#endif
