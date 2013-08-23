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
 * CVS-ID $Id: clock_ucs.c,v 1.8 2011/12/16 06:36:03 ari Exp $
 */

#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

#if defined(__MSP430_HAS_FLLPLUS__)

// See msp430f4619 datasheet for details.
// There are always two ranges that whould be
// suitable as they overlap.

static const uint16_t dcoRanges[] = { 2500,
                                      5000,
                                      10000,
                                      20000,
                                      46000 };

static void initFll(uint16_t fsystem, uint16_t fref)
{
  uint16_t flln, flld;
  uint8_t useUndividedDcoClock;
  uint16_t ratio = fsystem * 1000L / fref;
  volatile uint16_t x;

  flln = ratio;
  flld = 0;
  useUndividedDcoClock = 1;

// If fsystem / fref ratio is so big that FLLN bits cannot
// handle it, use prescaler to reduce it.

  while (flln > (1 << 7)) {

    flld = flld + 1;
    flln >>= 1;
  }

// If prescaler was not needed in previous step, we
// can switch to DCOCLKDIV instead of DCOCLK if DCO frequency allows.
// This can give less jitter on MCLK & SMCLK clocks.

  if (flld == 0) {

    useUndividedDcoClock = 0;

    while (fsystem <= 16000 && flld < 2) {

      flld = flld + 1;
      fsystem <<= 1;
    }
  }

// Before tweaking clock parameters, disable FLL.

  __bis_status_register(SCG0);

// Set FLL parameters. First put DCO to lowest tap,
// then set new FLLD and FLLN values.

  SCFI1 = 0x0000;
  SCFQCTL = (flln - 1);

// Set DCO range. Ranges are documented at datasheet.

  for (x = 0; x < sizeof(dcoRanges) / sizeof(uint16_t); x++)
    if (fsystem <= dcoRanges[x]) {

      SCFI0 = (flld << 6) | (1 << (x + 2));
      break;
    }

  P_ASSERT("fsystem", x <  sizeof(dcoRanges) / sizeof(uint16_t));

// Enable FLL again.

  __bic_status_register(SCG0);

// Wait for DCO fault flag to clear.

  while (FLL_CTL0 & DCOF) {

    FLL_CTL0 &= ~(DCOF);
    IFG1 &= ~OFIFG;
  }

  FLL_CTL0 = (FLL_CTL0 & ~(DCOPLUS)) | (useUndividedDcoClock ? DCOPLUS : 0);
  FLL_CTL1 = (FLL_CTL1 & ~(SELM0 + SELM1 + SELS)) | XT2OFF;

// Wait for DCO to settle, 32 * 32 * f_FLLREFCLK cycles.

  x = ratio * 32;
  while (x--){

    __delay_cycles(32);
  }
}

void portInitClock(void)
{
#if PORTCFG_XT1_HZ > 0

// Wait for XT1 fault flag to clear.

  while (FLL_CTL0 & LFOF) {

    FLL_CTL0 &= ~(LFOF);
    IFG1 &= ~OFIFG;
  }

#else

#error XT1 needed.

#endif

  initFll(PORTCFG_CPU_CLOCK_MHZ * 1000, PORTCFG_XT1_HZ);

// Wait for all fault flags to clear.

  do {

    FLL_CTL0 &= ~(DCOF + LFOF);
    IFG1 &= ~OFIFG;

  } while (IFG1 & OFIFG);
}

#endif
