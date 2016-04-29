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
#include <stdbool.h>
#include "port_aclk.h"

#if !defined(PORTCFG_TICK_TIMER_A0) || PORTCFG_TICK_TIMER_A0 == 1

#if POSCFG_FEATURE_TICKLESS != 0
#define CLK_DIVIDER ID_3 // divide ACLK by 8 to get longer sleeps, less accuracy
#define T0CYCLES_PER_TICK (PORT_ACLK_HZ / 8 / HZ)
#else
#define CLK_DIVIDER ID_0 // ACLK as-is
#define T0CYCLES_PER_TICK (PORT_ACLK_HZ / HZ)
#endif

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

  TA0CCR0 = T0CYCLES_PER_TICK - 1;

  TA0CCTL0 = CCIE;                    // Interrupts ON.
  TA0CTL |= TACLR;                    // Startup clear.
  TA0CTL |= CLK_DIVIDER;
  TA0CTL |= MC_1;                     // Up mode.

#else
#warning No TA0 timer, one required for ticks.
#endif

}

#if POSCFG_FEATURE_TICKLESS != 0

#ifndef PORTCFG_POWER_TICKLESS_SAFETY_MARGIN
#define PORTCFG_POWER_TICKLESS_SAFETY_MARGIN 0
#endif

static bool sleeping;

/*
 * Turn of timer tick and schedule wakeup
 * after given ticks.
 */
void p_pos_powerTickSuspend(UVAR_t ticks)
{
/*
 * First, stop timer.
 */
  TA0CTL &= ~MC_3;    // Stop timer.
  TA0CCTL0 &= ~CCIFG; // clear possible interrupt

/*
 * Then, schedule timer interrupt after ticks have passed.
 */

  uint16_t maxTicks;

  maxTicks = 65535 / T0CYCLES_PER_TICK;
  if (ticks == INFINITE || ticks > maxTicks)
    ticks = maxTicks;

  ticks -= PORTCFG_POWER_TICKLESS_SAFETY_MARGIN;

  TA0CCR0 = ticks * T0CYCLES_PER_TICK - 1;

  sleeping = true;
  TA0CTL |= MC_1;  // Up mode.
}

void p_pos_powerTickResume()
{
  TA0CTL   &= ~MC_3;  // Stop timer.
  TA0CCTL0 &= ~CCIFG; // clear possible interrupt

/*
 * Now step the timer with amount of ticks we really slept.
 */
  c_pos_timerStep(TA0R / T0CYCLES_PER_TICK);

/*
 * Enable timer now.
 */
  sleeping = false;

  TA0CCR0 = T0CYCLES_PER_TICK - 1;
  TA0CTL |= TACLR;                    // Startup clear.
  TA0CTL |= CLK_DIVIDER;
  TA0CTL |= MC_1;                     // Up mode.
}

#endif

#ifdef TIMER0_A0_VECTOR
void PORT_NAKED __attribute__((interrupt(TIMER0_A0_VECTOR))) timerIrqHandler()
#else
void PORT_NAKED __attribute__((interrupt(TIMERA0_VECTOR))) timerIrqHandler()
#endif
{
  portSaveContext();
  c_pos_intEnter();

#if POSCFG_FEATURE_TICKLESS != 0
  if (sleeping) {

    posCurrentTask_g->stackptr->sr &= ~LPM4_bits; // Wake up CPU
    TA0CTL &= ~MC_3;                              // Stop timer.
  }
  else
#endif
    c_pos_timerInterrupt();

  c_pos_intExit();
  portRestoreContext();
}

#endif
