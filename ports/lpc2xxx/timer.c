/*
 * Copyright (c) 2006-2013, Ari Suutari <ari@stonepile.fi>.
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
#include "lpc_reg.h"

static void Tick_Handler(void);

/*
 * Initialize timer..
 */

void
portInitTimer(void)
{
/*
 * Configure timer so that it ticks every 1 ms.
 */

  T0_PR = 0x0;

  T0_MR0 = PORTCFG_CRYSTAL_CLOCK / HZ;
  T0_MCR = T_MCR_MR0R | T_MCR_MR0I;   /* Reset & interrupt on match */

/*
 * Configure VIC to handle timer interrupts.
 */

  VIC_IntSelect &= ~(VIC_IntSelect_Timer0);
  VIC_IntEnable |= VIC_IntEnable_Timer0;
  VIC_VectAddr0 = (unsigned long)Tick_Handler;
  VIC_VectCntl0 = VIC_Channel_Timer0 | VIC_VectCntl_ENABLE;

/*
 * Start timer !
 */

  T0_TCR = T_TCR_CE;
}

/*
 * Timer interrupt handler. Just call generic pico]OS timer function
 * and enable the timer again.
 */

static void
Tick_Handler()
{
  portSaveContext();
  c_pos_intEnter();
  c_pos_timerInterrupt();

  T0_IR = T_IR_MR0;
  VIC_VectAddr = 0;

  c_pos_intExit();
  portRestoreContext();
}

