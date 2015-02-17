/*
 * Copyright (c) 2014-2015, Ari Suutari <ari@stonepile.fi>.
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
#include <string.h>

#if !defined(PORTCFG_TICK_CORE_TIMER) || PORTCFG_TICK_CORE_TIMER == 1

/*
 * Simple timer source based on MIPS core timer. This is
 * not a perfect RTOS tick source, as it will always
 * drift a little (because the way it is implemented in MIPS CPU).
 */

void portInitClock(void)
{
  IFS0CLR = _IFS0_CTIF_MASK; // Clear core timer interrupt flag
  IPC0CLR = _IPC0_CTIP_MASK |  _IPC0_CTIS_MASK; // clear and set priority & subpri
  IPC0SET = 2 << _IPC0_CTIP_POSITION;
  IEC0SET = 1 << _IEC0_CTIE_POSITION; // enable interrupt

  _CP0_SET_COUNT(0);
  _CP0_SET_COMPARE((PORTCFG_CRYSTAL_CLOCK / 2 / HZ));
}

/*
 * Timer interrupt from core timer.
 */

void  PORT_NAKED __attribute__((vector(_CORE_TIMER_VECTOR))) CoreTimerHandler(void)
{
  portSaveContext();
  c_pos_intEnter();

  // Clear the interrupt and
  // prepare counter compare register
  // for next cycle.

  IFS0CLR = _IFS0_CTIF_MASK;
  _CP0_SET_COUNT(0);
  _CP0_SET_COMPARE((PORTCFG_CRYSTAL_CLOCK / 2 / HZ));
  c_pos_timerInterrupt();

  c_pos_intExitQuick();
  portRestoreContext();
}

#endif
