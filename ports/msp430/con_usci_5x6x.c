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
 * CVS-ID $Id: con_usci_5x6x.c,v 1.4 2011/12/30 14:29:16 ari Exp $
 */

#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
#if defined(__MSP430_HAS_USCI_A0__) && (PORTCFG_CON_PERIPH == 0 || PORTCFG_CON_PERIPH == 1)


#if NOSCFG_FEATURE_CONIN == 1
static void uartRx(void);
#endif

static void uartTx(void);
void uartIrqHandler(void);

void portInitUSCIirq()
{
#if NOSCFG_FEATURE_CONIN == 1

  UCA0IE |= UCRXIE; // Enable USCI_A0 RX interrupt

#endif
}

void PORT_NAKED __attribute__((interrupt(USCI_A0_VECTOR))) uartIrqHandler()
{
  portSaveContext();
  c_pos_intEnter();

  switch (UCA0IV)
  {
#if NOSCFG_FEATURE_CONIN == 1
    case USCI_UCRXIFG: // RX
      uartRx();
      break;
#endif

    case USCI_UCTXIFG: // TX
      uartTx();
      break;
  }

  c_pos_intExit();
  portRestoreContext();
}


#if NOSCFG_FEATURE_CONIN == 1
static void uartRx()
{
  char ch = UCA0RXBUF;

  c_nos_keyinput(ch);
}
#endif

static void uartTx()
{
  UCA0IE &= ~UCTXIE; // Disable TX interrupt

#if NOSCFG_CONOUT_HANDSHAKE == 1 && NOSCFG_FEATURE_CONOUT == 1
  c_nos_putcharReady();
#endif
}

#if NOSCFG_FEATURE_CONOUT == 1

/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
#if NOSCFG_CONOUT_HANDSHAKE != 0

  if (UCA0IE & UCTXIE) // If interrupt is enabled, previous char is not yet transmitted.
    return 0;

  UCA0TXBUF = c;
  UCA0IE |= UCTXIE; // Enable TX interrupt

#else

  while (!(UCA0IFG&UCTXIFG)); // Wait for TX buffer to empty
  UCA0TXBUF = c;

#endif

  return 1;
}

#endif
#endif
#endif

