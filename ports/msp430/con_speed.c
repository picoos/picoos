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
 * CVS-ID $Id: arch_c.c,v 1.91 2012/01/19 09:46:49 ari Exp $
 */

#include <picoos.h>
#include <msp430.h>
#include <in430.h>
#include <string.h>

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

//
// How to calculate USART parameters:
// 0: Fixed table, supports only some clock speeds and 9600 baud.
// 1: TI algorithm, accurate but uses floating point. Adds 3.4 kB to code size.
//    Also stack usage at startup is bigger.
// 2: Simplified algoritum, integer aritmethic. Accurate enough for SMCLK speeds > 1 Mhz.

#define SPEED_CALC 2

#if SPEED_CALC != 0 || defined(__MSP430_HAS_UART1__)

const uint8_t modPatterns[8] = {

        // See USCI documentation for these batterns.
        // bit0  bit1  bit2  bit3  bit4  bit5  bit6  bit7
  0x00, //  0,    0,    0,    0,    0,    0,    0,    0
  0x02, //  0,    1,    0,    0,    0,    0,    0,    0
  0x22, //  0,    1,    0,    0,    0,    1,    0,    0
  0x2a, //  0,    1,    0,    1,    0,    1,    0,    0
  0xaa, //  0,    1,    0,    1,    0,    1,    0,    1
  0xae, //  0,    1,    1,    1,    0,    1,    0,    1
  0xee, //  0,    1,    1,    1,    0,    1,    1,    1
  0xfe  //  0,    1,    1,    1,    1,    1,    1,    1
};

#endif

#if SPEED_CALC == 0
void portConSpeed(uint32_t brClock, uint32_t baudRate, uint16_t* brDiv, uint16_t* brMod)
{
#if (PORTCFG_CPU_CLOCK_MHZ == 1)

  *brDiv  = 0x0068;                           // 9600 from 1Mhz
  *brMod  = 1;

#elif (PORTCFG_CPU_CLOCK_MHZ == 8)

  *brDiv = 0x0341;                            // 9600 from 8Mhz
  *brMod = 2;

#elif (PORTCFG_CPU_CLOCK_MHZ == 12)

  *brDiv  = 0x04E2;                           // 9600 from 12Mhz
  *brMod  = 0;

#elif (PORTCFG_CPU_CLOCK_MHZ == 16)

  *brDiv  = 0x0682;                           // 9600 from 16Mhz
  *brMod  = 6;

#elif (PORTCFG_CPU_CLOCK_MHZ == 20)

  *brDiv  = 0x0823;                           // 9600 from 20Mhz
  *brMod  = 2;

#else
#error "ERROR: Unsupported CPU clock speed."
#endif
}
#endif

#if SPEED_CALC == 1

#define BITS (1 + 8 + 1)

static double bitErrorRate(uint32_t baudRate, uint32_t brClock, uint16_t brDiv, uint16_t brMod)
{
  double maxError = 0;
  int i;
  double ideal = 0;
  double modulated = 0;
  double bitError;
  uint8_t modBit;

  for (i = 0; i < BITS; i++) {

    ideal = ideal + (1.0 / baudRate);
    modBit = modPatterns[brMod] >> (i % 8);
    modBit &= 0x1;
    modulated = modulated + ((1.0 / brClock) * (brDiv + modBit));


    bitError = (modulated - ideal) * baudRate * 100;

    if (bitError < 0)
      bitError = -bitError;

    if (i == 0 || bitError > maxError)
       maxError = bitError;
  }

  return maxError;
}

void portConSpeed(uint32_t brClock, uint32_t baudRate, uint16_t* brDiv, uint16_t* brMod)
{
  int tryMod;
  double brDivFloat;
  double error;
  double minError = 100.0;
  uint16_t calcMod;

  *brMod = -1;
  brDivFloat = (1.0 * brClock) / baudRate;
  *brDiv = (unsigned int)brDivFloat;
  calcMod = (uint16_t)((brDivFloat - *brDiv) * 8 + 0.5);

  tryMod = calcMod - 1;
  if (tryMod < 0)
    tryMod = 0;

  for (; tryMod < 8 && tryMod <= calcMod + 1; tryMod++) {

    error = bitErrorRate(baudRate, brClock, *brDiv, tryMod);
    if (error < minError) {

      *brMod = tryMod;
      minError = error;
    }
  }
}
#endif

#if SPEED_CALC == 2
void portConSpeed(uint32_t brClock, uint32_t baudRate, uint16_t* brDiv, uint16_t* brMod)
{
  *brDiv = brClock / baudRate;
  *brMod = ((16L * brClock) / baudRate - 16 * *brDiv + 1) / 2;
}
#endif

#if defined(__MSP430_HAS_UART1__)
void portConSpeedModPattern(uint32_t brClock, uint32_t baudRate, uint16_t* brDiv, uint8_t* brModPattern)
{
  uint16_t brMod;

  portConSpeed(brClock, baudRate, brDiv, &brMod);
  *brModPattern = modPatterns[brMod];
}

#endif

#endif
