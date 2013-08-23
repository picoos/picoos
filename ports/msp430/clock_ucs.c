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

#if defined(__MSP430_HAS_UCS_RF__) || defined(__MSP430_HAS_UCS__)

// See cc430F5137 datasheet for details.
// There are always two ranges that whould be
// suitable as they overlap.

static const uint16_t dcoRanges[] = { 630,
                                      1250,
                                      2500,
                                      5000,
                                      10000,
                                      20000,
                                      35000,
                                      60000 };

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
//
// Use 9 flln bits, cpu has 10 but TI examples use only 9.
// Discussion about this:
//
// http://e2e.ti.com/support/microcontrollers/msp43016-bit_ultra-low_power_mcus/f/166/t/123555.aspx

  while (flln > (1 << 9)) {

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

  UCSCTL0 = 0x0000;
  UCSCTL2 &= ~(0x03FF);
  UCSCTL2 = (flld << 12) | (flln - 1);

// Set DCO range. Ranges are documented at datasheet,
// and there is some documentation about this at:
// http://e2e.ti.com/support/microcontrollers/msp43016-bit_ultra-low_power_mcus/f/166/t/121007.aspx

  for (x = 0; x < sizeof(dcoRanges) / sizeof(uint16_t); x++)
    if (fsystem <= dcoRanges[x]) {

      UCSCTL1 = (x << 4);
      break;
    }

  P_ASSERT("fsystem", x <  sizeof(dcoRanges) / sizeof(uint16_t));

// Enable FLL again.

  __bic_status_register(SCG0);

// Wait for DCO fault flag to clear.

  while (UCSCTL7 & DCOFFG) {

    UCSCTL7 &= ~DCOFFG;
    SFRIFG1 &= ~OFIFG;
  }

  if (useUndividedDcoClock) {
    UCSCTL4 = (UCSCTL4 & ~(SELM_7 + SELS_7)) | (SELM__DCOCLK + SELS__DCOCLK);
  }
  else {
    UCSCTL4 = (UCSCTL4 & ~(SELM_7 + SELS_7)) | (SELM__DCOCLKDIV + SELS__DCOCLKDIV);
  }

// Wait for DCO to settle, 32 * 32 * f_FLLREFCLK cycles.

  x = ratio * 32;
  while (x--){

    __delay_cycles(32);
  }
}

void portInitClock(void)
{
  uint16_t pmmStatus;

#if PORTCFG_XT1_HZ > 0

  UCSCTL6 &= (~XT1OFF);     // Turns on the XT1.
  UCSCTL6 |= XT1DRIVE_3;            // Highest setting for startup.

// Wait for XT1 fault flag to clear.

  while (UCSCTL7 & XT1LFOFFG) {

    UCSCTL7 &= ~(XT1LFOFFG);
    SFRIFG1 &= ~OFIFG;
  }

  UCSCTL6 = (UCSCTL6 & ~(XT1DRIVE_3)) | (XT1DRIVE_0); // Reduce drive after startup
  UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | SELA__XT1CLK;
 
#else

  UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | SELA__VLOCLK;

#endif

  // Setup VCore at correct level. Taken from
  // cc430f5137 datasheet.

#if (PORTCFG_CPU_CLOCK_MHZ > 16)

  pmmStatus = portSetVCore(PMMCOREV_3);

#elif (PORTCFG_CPU_CLOCK_MHZ > 12)

  pmmStatus = portSetVCore(PMMCOREV_2);

#elif (PORTCFG_CPU_CLOCK_MHZ > 8)

  pmmStatus = portSetVCore(PMMCOREV_1);

#else

  pmmStatus = portSetVCore(PMMCOREV_0);

#endif

  if (pmmStatus == 0) {

#if PORTCFG_XT1_HZ > 0

    UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | SELREF__XT1CLK;

    initFll(PORTCFG_CPU_CLOCK_MHZ * 1000, PORTCFG_XT1_HZ);

#else
  
    UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | SELREF__REFOCLK;

    initFll(PORTCFG_CPU_CLOCK_MHZ * 1000, 32768);

#endif

  }

#if PORTCFG_XT1_HZ > 0
  SFRIE1 |= OFIE;
#endif
}

static void __attribute__((interrupt(UNMI_VECTOR))) nmiIsr(void)
{
// If it still can't clear the oscillator fault flags after the timeout, trap and wait here.

  do {

    UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG);

// CC430 requires 3 VLO clock cycles delay.
// If 20MHz CPU, 5000 clock cycles are required in worst case.
// Wait won't hurt for other cpus, so do it always.

    __delay_cycles(5000);

    SFRIFG1 &= ~OFIFG;

  } while (SFRIFG1 & OFIFG);

}

#endif

