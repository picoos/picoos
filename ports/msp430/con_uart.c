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
 * CVS-ID $Id: arch_c.c,v 1.91 2012/01/19 09:46:49 ari Exp $
 */

#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
#if defined(__MSP430_HAS_UART1__) && (PORTCFG_CON_PERIPH == 2)


/*
 * Initialize USCI uart.
 */

void portInitConsole(void)
{
  U1CTL |= SWRST;

  U1ME |= UTXE1 + URXE1;
  U1CTL |= CHAR;
  U1TCTL |= SSEL1 + SSEL0 + URXSE;

  uint16_t brDiv;
  uint8_t brModPat;

  portConSpeedModPattern(PORTCFG_CPU_CLOCK_MHZ * 1000000L, 9600, &brDiv, &brModPat);

  U1BR0 = (brDiv & 0xff);
  U1BR1 = (brDiv >> 8);
  U1MCTL = brModPat;

  U1CTL &= ~SWRST;

#if NOSCFG_FEATURE_CONIN == 1
  IE2 |= URXIE1; // Enable RX interrupt
#endif
}

#if NOSCFG_CONOUT_HANDSHAKE != 0
void uartTxIrqHandler(void);
#endif

void uartRxIrqHandler(void);

#if NOSCFG_FEATURE_CONOUT == 1

/*
 * Uart interrupt handler.
 */

#if NOSCFG_CONOUT_HANDSHAKE != 0
void PORT_NAKED __attribute__((interrupt(USART1TX_VECTOR))) uartTxIrqHandler()
{
  portSaveContext();
  c_pos_intEnter();

  IE2 &= ~UTXIE1; // Disable TX interrupt
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

  if (IE2 & UTXIE1) // If interrupt is still enabled the uart is busy
    return 0;

  U1TXBUF = c;
  IE2 |= UTXIE1; // Enable TX interrupt

#else

  while (!(U1IFG & UTXIFG1)); // Wait for char to be transmitted
  U1TXBUF = c;

#endif

  return 1;
}
#endif

#if NOSCFG_FEATURE_CONIN == 1

void __attribute__((interrupt(USART1RX_VECTOR))) uartRxIrqHandler()
{
  portSaveContext();
  c_pos_intEnter();

  if (U1IFG & URXIFG1) {

    char ch = U1RXBUF;
    c_nos_keyinput(ch);
    U1TCTL |= SSEL0;
  }
  else {

    U1TCTL &= ~URXSE;  // Clear URXS signal and enable it again.
    U1TCTL |= URXSE;
    U1TCTL &= ~SSEL0;
  }


  c_pos_intExit();
  portRestoreContext();
}

#endif

#endif
#endif
