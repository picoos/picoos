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
 * CVS-ID $Id: arm7pid_reg.h,v 1.2 2006/04/29 15:25:54 dkuschel Exp $
 */

#define Timer0Load     (*((volatile unsigned long *) 0xA800000))
#define Timer0Value    (*((volatile unsigned long *) 0xA800004))
#define Timer0Ctrl     (*((volatile unsigned long *) 0xA800008))
#define Timer0Clr      (*((volatile unsigned long *) 0xA80000C))

#define Timer1Load     (*((volatile unsigned long *) 0xA800020))
#define Timer1Value    (*((volatile unsigned long *) 0xA800024))
#define Timer1Ctrl     (*((volatile unsigned long *) 0xA800028))
#define Timer1Clr      (*((volatile unsigned long *) 0xA80002C))

#define IrqStatus      (*((volatile unsigned long *) 0xA000000))
#define IrqRawStatus   (*((volatile unsigned long *) 0xA000004))
#define IrqEnable      (*((volatile unsigned long *) 0xA000008))
#define IrqEnableClear (*((volatile unsigned long *) 0xA00000C))
#define IrqSoft        (*((volatile unsigned long *) 0xA000010))
#define FiqStatus      (*((volatile unsigned long *) 0xA000100))
#define FiqRawStatus   (*((volatile unsigned long *) 0xA000104))
#define FiqEnable      (*((volatile unsigned long *) 0xA000108))
#define FiqEnableClear (*((volatile unsigned long *) 0xA00010C))

