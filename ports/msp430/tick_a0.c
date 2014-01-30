/*
 * Copyright (c) 2011-2014, Ari Suutari <ari@stonepile.fi>.
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
#include <msp430.h>
#include <in430.h>
#include <string.h>

#if !defined(PORTCFG_TICK_TIMER_A0) || PORTCFG_TICK_TIMER_A0 == 1

void timerIrqHandler(void);

/*
 * Initialize timer.
 */

void portInitTimer(void)
{
#if defined(__MSP430_HAS_TA3__) || defined(__MSP430_HAS_T0A5__) || defined(__MSP430_HAS_TA2__)

  TA0CTL = 0;                         // Stop timer.
  TA0CTL = TASSEL_1;                  // Use ACLK.
  TA0CTL |= TACLR;                    // Clear everything.

#if defined(PORTCFG_XT1_HZ) && PORTCFG_XT1_HZ > 0

  TA0CCR0 = (PORTCFG_XT1_HZ / HZ) - 1;      // Using crystal XT1

#else

#if defined(__msp430x22x4) || defined(__msp430x22x2) || defined(__MSP430G2553__) || defined(__MSP430G2533__)

  TA0CCR0 = (12000 / HZ) - 1;      // VLO on msp430x2xx is 12 Khz

#elif defined(__cc430x513x)

  TA0CCR0 = (10000 / HZ) - 1;      // VLO on msp430x5xx is 10 Khz

#else

#warning VLO frequency unknown, assuming 12 Khz

  TA0CCR0 = (12000 / HZ) - 1;      // VLO on msp430x2xx is 12 Khz

#endif

#endif

  TA0CCTL0 = CCIE;                    // Interrupts ON.
  TA0CTL |= TACLR;                    // Startup clear.
  TA0CTL |= MC_1;                     // Up mode.

#else
#warning No TA0 timer, one required for ticks.
#endif

}

#ifdef TIMER0_A0_VECTOR
void PORT_NAKED __attribute__((interrupt(TIMER0_A0_VECTOR))) timerIrqHandler()
#else
void PORT_NAKED __attribute__((interrupt(TIMERA0_VECTOR))) timerIrqHandler()
#endif
{
  portSaveContext();
  c_pos_intEnter();
  c_pos_timerInterrupt();
  c_pos_intExit();
  portRestoreContext();
}

#endif
