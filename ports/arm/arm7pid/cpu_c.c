/*
 * Copyright (c) 2004, Jun Li, lj_sourceforge@users.sourceforge.net.
 * Copyright (c) 2004, Dennis Kuschel. All rights reserved.
 *
 * Modified for new arm port by Ari Suutari, ari@suutari.iki.fi.
 * UNTESTED - I HAVE NO HARDWARE FOR THIS.
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
 * CVS-ID $Id: cpu_c.c,v 1.2 2006/04/29 15:25:54 dkuschel Exp $
 */

#define NANOINTERNAL
#include <picoos.h>
#include "arm7pid_reg.h"

static PortIrqHandlerFunction defaultIrqHandler;
void portCpuIrqHandler(int irq);

/*
 * Initialize CPU pins, clock and console.
 */

void
p_pos_initArch(void)
{
  defaultIrqHandler = NULL;
  Timer1Ctrl = 0x44;		// Periodic / prescale_16
  Timer1Load = 150000;		// Load
  Timer1Ctrl = 0xC4;		// Enable
  IrqEnable  = 0x20;		// Enable Time IRQ
}

/*
 * Route interupt to correct handler (called from assembly).
 */

void portCpuIrqHandler(int irq)
{
  switch (irq) {
  case 0x20:
    c_pos_timerInterrupt();
    Timer1Clr = 0;
    break;

  default:
    if (defaultIrqHandler != NULL)
        (*defaultIrqHandler)(irq);
  }
}
  
void portSetDefaultIrqHandler(PortIrqHandlerFunction func)
{
  defaultIrqHandler = func;
}
