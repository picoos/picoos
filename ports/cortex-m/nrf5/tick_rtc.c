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

#include "nrf_rtc.h"
#include "nrf_clock.h"

void portInitClock(void)
{
  nrf_rtc_prescaler_set(NRF_RTC1, RTC_FREQ_TO_PRESCALER(HZ));
  nrf_rtc_event_enable(NRF_RTC1, RTC_EVTENSET_TICK_Msk);
  nrf_rtc_int_enable(NRF_RTC1, RTC_INTENSET_TICK_Msk);

  NVIC_SetPriority(RTC1_IRQn, PORT_SYSTICK_PRI);
  NVIC_EnableIRQ(RTC1_IRQn); 

  nrf_rtc_task_trigger(NRF_RTC1, NRF_RTC_TASK_START);
}

void RTC1_IRQHandler()
{
  c_pos_intEnter();
  if (nrf_rtc_event_pending(NRF_RTC1, NRF_RTC_EVENT_TICK)) {

    nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_TICK);
    c_pos_timerInterrupt();
  }

  c_pos_intExitQuick();
}

