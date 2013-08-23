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
 * CVS-ID $Id: bsp.c,v 1.2 2011/12/05 10:19:57 ari Exp $
 */

#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

/*
 * Initialize board pins.
 */

void portInitBoard(void)
{

#if PORTCFG_XT1_HZ > 0

#if defined(__cc430x513x)

  P5OUT = 0x00;
  P5SEL |= BIT1 + BIT0;

  UCSCTL6 &= (~XTS);        // Select Low-frequency mode.
  UCSCTL6 |= XCAP_3;        // Internal load cap.

#elif defined(__msp430xG46x)

  FLL_CTL0 &= (~XTS_FLL);   // Select Low-frequency mode.
  FLL_CTL0 |= OSCCAP_3;     // Set load capacitance for xtal

#else
#error dont know which pins are connected to XT1
#endif

#endif

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

/*
 * Program * TX & RX pin for usart use.
 */

  UCA0CTL1 |= UCSWRST;

#if defined(__msp430x22x4) || defined(__msp430x22x2)

  P3DIR |= BIT4;                            // Set P3.4 as TX output
  P3SEL |= 0x30;            // P3.4,5 = USCI_A0 TXD/RXD

#elif defined(__cc430x513x)

  P1DIR |= BIT6;                            // Set P1.6 as TX output
  P1SEL |= BIT5 + BIT6;                     // Select P1.5 & P1.6 to UART function

#elif defined(__msp430xG46x)

  P4DIR |= BIT0;                            // Set P4.0 as TX output
  P4SEL |= BIT0 + BIT1;                     // Select P4.0 & P4.1 to UART function

#else

#error missing usart tx/rx pinout

#endif
#endif
}
