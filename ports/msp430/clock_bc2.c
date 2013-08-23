/*
 * Copyright (c) 2011, Ari Suutari, ari@suutari.iki.fi.
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
 * CVS-ID $Id: clock_bc2.c,v 1.1 2011/12/05 10:29:55 ari Exp $
 */

#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

#ifdef __MSP430_HAS_BC2__

void portInitClock()
{

/*
 * Based on PORTCFG_CPU_CLOCK_MHZ, pick right factory
 * calibrated constant from flash.
 */
#if (PORTCFG_CPU_CLOCK_MHZ == 1)

  BCSCTL1   = CALBC1_1MHZ;
  DCOCTL    = CALDCO_1MHZ;

#elif (PORTCFG_CPU_CLOCK_MHZ == 8)

  BCSCTL1   = CALBC1_8MHZ;
  DCOCTL    = CALDCO_8MHZ;

#elif (PORTCFG_CPU_CLOCK_MHZ == 12)

  BCSCTL1   = CALBC1_12MHZ;
  DCOCTL    = CALDCO_12MHZ;

#elif (PORTCFG_CPU_CLOCK_MHZ == 16)

  BCSCTL1   = CALBC1_16MHZ;
  DCOCTL    = CALDCO_16MHZ;

#else
#error "ERROR: Unsupported CPU clock speed."
#endif

#if !defined(PORTCFG_XT1_HZ) || PORTCFG_XT1_HZ == 0

  BCSCTL3 |= LFXT1S_2; // use VLO for ACLK if there is no XT1 crystal

#endif
}
#endif

