/*
 * Copyright (c) 2011, Ari Suutari, ari@suutari.iki.fi.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 2. Redistributions
 * in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution. 3. The name of the author may
 * not be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 * 
 * CVS-ID $Id: con_usci.c,v 1.5 2011/12/30 16:00:02 ari Exp $
 */

#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
#if defined(__MSP430_HAS_USCI__) && (PORTCFG_CON_PERIPH == 0 || PORTCFG_CON_PERIPH == 1)

#if NOSCFG_CONOUT_HANDSHAKE != 0
void uartTxIrqHandler(void);
#endif

void uartRxIrqHandler(void);

void portInitUSCIirq()
{
#if NOSCFG_FEATURE_CONIN == 1

  UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt

#endif
}

#if NOSCFG_FEATURE_CONOUT == 1

/*
 * Uart interrupt handler.
 */

#if NOSCFG_CONOUT_HANDSHAKE != 0
void PORT_NAKED __attribute__((interrupt(USCIAB0TX_VECTOR))) uartTxIrqHandler()
{
  portSaveContext();
  c_pos_intEnter();

  UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
  c_nos_putcharReady();

  c_pos_intExit();
  portRestoreContext();
}
#endif

/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
#if NOSCFG_CONOUT_HANDSHAKE != 0

  if (UC0IE & UCA0TXIE) // if interrupt is enabled then previous char is being processed.
    return 0;

  UCA0TXBUF = c;
  UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt

#else

  while (!(UC0IFG & UCA0TXIFG)); // Wait for char to be transmitted
  UCA0TXBUF = c;

#endif

  return 1;
}
#endif

#if NOSCFG_FEATURE_CONIN == 1

void __attribute__((interrupt(USCIAB0RX_VECTOR))) uartRxIrqHandler()
{
  portSaveContext();
  c_pos_intEnter();

  char ch = UCA0RXBUF;
  c_nos_keyinput(ch);

  c_pos_intExit();
  portRestoreContext();
}

#endif

#endif
#endif
