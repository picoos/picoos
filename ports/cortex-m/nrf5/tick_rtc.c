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

#include "nrf_rtc.h"
#include "nrf_clock.h"

void portInitClock(void)
{
  nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_STOP);
  nrf_rtc_prescaler_set(NRF_RTC1, RTC_FREQ_TO_PRESCALER(HZ));
  nrf_rtc_event_enable(NRF_RTC1, NRF_RTC_EVENT_TICK);
  nrf_rtc_int_enable(NRF_RTC1, NRF_RTC_INT_TICK_MASK);

  NVIC_SetPriority(RTC1_IRQn, PORT_SYSTICK_PRI);
  NVIC_EnableIRQ(RTC1_IRQn);

  nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_START);
}

#if POSCFG_FEATURE_TICKLESS != 0

#ifndef PORTCFG_POWER_TICKLESS_SAFETY_MARGIN
#define PORTCFG_POWER_TICKLESS_SAFETY_MARGIN MS(5)
#endif

static uint32_t sleepStart;

/*
 * Turn of timer tick and schedule wakeup
 * after given ticks.
 */
void p_pos_powerTickSuspend(UVAR_t ticks)
{
  uint32_t sleepEnd;
/*
 * First, turn periodic tick off.
 */
  nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_STOP);
  nrf_rtc_int_disable(NRF_RTC1, NRF_RTC_INT_TICK_MASK);
  nrf_rtc_event_disable(NRF_RTC1, NRF_RTC_EVENT_TICK);

/*
 * Then, schedule RTC wakeup after ticks have passed.
 */
  if (ticks == INFINITE || ticks > RTC_COUNTER_COUNTER_Msk) // max 2^24
    ticks = RTC_COUNTER_COUNTER_Msk;

  ticks -= PORTCFG_POWER_TICKLESS_SAFETY_MARGIN; // oscillator starts up 2 ms

  sleepStart = nrf_rtc_counter_get(NRF_RTC1);
  sleepEnd = (sleepStart + ticks) & 0x00ffffff;
  nrf_rtc_cc_set(NRF_RTC1, 0, sleepEnd);

  nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);
  nrf_rtc_event_enable(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);
  nrf_rtc_int_enable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);
  nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_START);
}

void p_pos_powerTickResume()
{
  int32_t delta;
/*
 * Make sure that rtc compare wakeup is disabled.
 */
  nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);
  nrf_rtc_int_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);
  nrf_rtc_event_disable(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);

/*
 * Now step the timer with amount of ticks we really slept.
 */
  delta = (nrf_rtc_counter_get(NRF_RTC1) - sleepStart) & 0x00ffffff;
  c_pos_timerStep(delta);

/*
 * Safe to enable tick now.
 */
  nrf_rtc_event_enable(NRF_RTC1, NRF_RTC_EVENT_TICK);
  nrf_rtc_int_enable(NRF_RTC1, NRF_RTC_INT_TICK_MASK);
}

#endif

void RTC1_IRQHandler()
{
#if POSCFG_FEATURE_TICKLESS != 0
  if (nrf_rtc_event_pending(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0)) {

    nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);
    nrf_rtc_int_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);
    nrf_rtc_event_disable(NRF_RTC1, NRF_RTC_EVENT_COMPARE_0);
    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
    return;
  }
#endif

  c_pos_intEnter();
  if (nrf_rtc_event_pending(NRF_RTC1, NRF_RTC_EVENT_TICK)) {

    nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_TICK);
    c_pos_timerInterrupt();
  }

  c_pos_intExitQuick();
}

