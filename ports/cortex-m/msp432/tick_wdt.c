/*
 * Copyright (c) 2015, Ari Suutari <ari@stonepile.fi>.
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
#include "driverlib.h"

#if PORTCFG_TICK_WDT == 1

/*
 * Initialize wdt as tick source.
 */

void portInitClock(void)
{
  uint16_t div;

  switch (CS_getBCLK() / HZ) {
  case 32768:
    div = WDT_A_CLOCKITERATIONS_32K;
    break;

  case 8192:
    div = WDT_A_CLOCKITERATIONS_8192;
    break;

  case 512:
    div = WDT_A_CLOCKITERATIONS_512;
    break;

  case 64:
    div = WDT_A_CLOCKITERATIONS_64;
    break;

  default:
    P_ASSERT("WDT cannot produce requested HZ", false);
    div = 0;
    break;
  }

  MAP_WDT_A_initIntervalTimer(WDT_A_CLOCKSOURCE_XCLK, div);

  NVIC_EnableIRQ(WDT_A_IRQn);
  NVIC_SetPriority(WDT_A_IRQn, PORT_SYSTICK_PRI);

  MAP_WDT_A_startTimer();
}

/*
 * Timer interrupt from WDT.
 */
void WDT_Handler(void)
{
  c_pos_intEnter();
  c_pos_timerInterrupt();
  c_pos_intExitQuick();
}

#endif
