/*
 * Copyright (c) 2006-2014, Ari Suutari <ari@stonepile.fi>.
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
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/*
 * Initialize Tiva uart module.
 */

#if PORTCFG_CON_USART > 0

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

void portInitConsole(void)
{
  //
  // Configure speed & parity.
  //
  UARTConfigSetExpClk(PORTCFG_CON_USART, SystemCoreClock, PORTCFG_CONSOLE_SPEED,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

  // Console shouldn't be realtime-critical,
  // use low interrupt priority for it.
  NVIC_ClearPendingIRQ(UART0_IRQn);
  NVIC_SetPriority(UART0_IRQn, PORT_PENDSV_PRI - 1);
  NVIC_EnableIRQ(UART0_IRQn);
  UARTIntEnable(PORTCFG_CON_USART, UART_INT_TX);

#if NOSCFG_FEATURE_CONIN == 1
  UARTIntEnable(PORTCFG_CON_USART, UART_INT_RX | UART_INT_RT);
#endif
}

/*
 * Usart interrupt handler.
 */

void UART0_Handler()
{
  c_pos_intEnter();

  uint32_t status;

  status = UARTIntStatus(PORTCFG_CON_USART, true);
  UARTIntClear(PORTCFG_CON_USART, status);

  if (status & UART_INT_TX)
    c_nos_putcharReady();

#if NOSCFG_FEATURE_CONIN == 1
  if (status & (UART_INT_RX | UART_INT_RT)) {

    unsigned char ch;

    while (UARTCharsAvail(PORTCFG_CON_USART)) {

      ch = UARTCharGetNonBlocking(PORTCFG_CON_USART);
      c_nos_keyinput(ch);
    }
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
  return UARTCharPutNonBlocking(PORTCFG_CON_USART, c);
}
#endif

#endif
#endif
