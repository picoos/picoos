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
 * CVS-ID $Id: samsung_reg.h,v 1.1 2006/04/29 15:32:44 dkuschel Exp $
 */

#define     VPint                   *(volatile unsigned int *)

#define Base_Addr       0xf0000000
#define TMOD            (Base_Addr+0x40000)               // Timer mode register    
#define TDATA(channel)  (Base_Addr+0x40010 + channel*0x8) // Timer  data register
#define	TIC		(Base_Addr+0x040004)
#define INTMASK         (Base_Addr+0x140008)            //  Internal Interrupt mask register       
#define EXTMASK         (Base_Addr+0x14000C)            //  External Interrupt mask register       

#define     gBUSCLK         133000000

#define     TM_RUN(device)              (0x1<<(device*4))
#define     TM_TOGGLE(device)           (0x2<<(device*4))
#define     TimerStart(device)          ((VPint(TMOD)) |= TM_RUN(device))
#define     TimerInterval(device)       ((VPint(TMOD)) &= ~TM_TOGGLE(device))

#define     Enable_Int(n)       (VPint(INTMASK))    &= (~((unsigned int)1<<(n)))
#define     GlobalEn_Int()      (VPint(EXTMASK))    &= (~((unsigned int)1<<31))
#define     TIMER0_INT          23
