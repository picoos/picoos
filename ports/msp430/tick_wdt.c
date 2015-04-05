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
#include "port_aclk.h"

#if PORTCFG_TICK_WDT == 1

void timerIrqHandler(void);

/*
 * Initialize timer.
 */

void portInitTimer(void)
{
#if HZ == (PORT_ACLK_HZ / 32768)
  WDTCTL = WDT_ADLY_1000;
#elif HZ == (PORT_ACLK_HZ / 8192)
  WDTCTL = WDT_ADLY_250;
#elif HZ == (PORT_ACLK_HZ / 512)
  WDTCTL = WDT_ADLY_16;
#elif HZ == (PORT_ACLK_HZ / 64)
  WDTCTL = WDT_ADLY_1_9;
#else
#error HZ must be ACLK frequency divided by 32768, 8192, 512 or 64.
#endif

  IE1 |= WDTIE;
}

void PORT_NAKED __attribute__((interrupt(WDT_VECTOR))) timerIrqHandler()
{
  portSaveContext();
  c_pos_intEnter();
  c_pos_timerInterrupt();
  c_pos_intExit();
  portRestoreContext();
}

#endif
