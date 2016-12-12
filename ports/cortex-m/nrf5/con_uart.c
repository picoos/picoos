/*
 * Copyright (c) 2016, Ari Suutari <ari@stonepile.fi>.
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

#include <picoos.h>
#include <port_irq.h>
#include "nrf_gpio.h"
#include "nrf_uart.h"


#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

void portInitConsole()
{
  nrf_uart_configure(NRF_UART0, NRF_UART_PARITY_EXCLUDED ,
                                NRF_UART_HWFC_DISABLED);

  nrf_uart_baudrate_set(NRF_UART0, NRF_UART_BAUDRATE_38400);
  nrf_uart_enable(NRF_UART0);

#if NOSCFG_FEATURE_CONOUT == 1
  nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTTX);
#endif
#if NOSCFG_FEATURE_CONIN == 1
  nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTRX);
  nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
  nrf_uart_int_enable(NRF_UART0, NRF_UART_INT_MASK_RXDRDY);
#endif

  // Console shouldn't be realtime-critical,
  // use low interrupt priority for it.
  NVIC_SetPriority(UART0_IRQn, PORT_CON_PRI);
  NVIC_EnableIRQ(UART0_IRQn);
}

#if NOSCFG_FEATURE_CONOUT == 1
UVAR_t p_putchar(char c)
{
  if (nrf_uart_int_enable_check(NRF_UART0, NRF_UART_INT_MASK_TXDRDY))
    return 0;

  nrf_uart_txd_set(NRF_UART0, c);
  nrf_uart_int_enable(NRF_UART0, NRF_UART_INT_MASK_TXDRDY);

  return 1;
}
#endif

void UART0_IRQHandler()
{
  unsigned char ch;

  c_pos_intEnter();

  if (nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_TXDRDY)) {

    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
    nrf_uart_int_disable(NRF_UART0, NRF_UART_INT_MASK_TXDRDY);
#if NOSCFG_FEATURE_CONOUT == 1
    c_nos_putcharReady();
#endif
  }

  if (nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_RXDRDY)) {

    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
#if NOSCFG_FEATURE_CONIN == 1
    ch = nrf_uart_rxd_get(NRF_UART0);
    c_nos_keyinput(ch);
#endif
  }

  c_pos_intExitQuick();
}

#endif
