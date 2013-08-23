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
#if (defined(__MSP430_HAS_USCI__) || defined(__MSP430_HAS_USCI_A0__)) && (PORTCFG_CON_PERIPH == 0 || PORTCFG_CON_PERIPH == 1)

/*
 * Initialize USCI uart.
 */

void portInitConsole(void)
{
  UCA0CTL1 |= UCSWRST;
  UCA0CTL1 = UCSSEL_2;      // SMCLK, running at MCLK

  uint16_t brDiv;
  uint16_t brMod;

  portConSpeed(PORTCFG_CPU_CLOCK_MHZ * 1000000L, 9600, &brDiv, &brMod);

  UCA0BR0 = (brDiv & 0xff);
  UCA0BR1 = (brDiv >> 8);
  UCA0MCTL = (brMod << 1);

  UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine

  portInitUSCIirq();

}

#endif
#endif
