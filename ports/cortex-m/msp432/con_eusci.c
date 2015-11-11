/*
 * Copyright (c) 2015, Ari Suutari <ari@stonepile.fi>.
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
#include "port_irq.h"
#include <stdbool.h>

#include "driverlib.h"

/*
 * Initialize MSP432 eusci module.
 */

#if PORTCFG_CON_EUSCI > 0

#define PORT EUSCI_A0
#define IRQ EUSCIA0_IRQn

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

void portInitConsole(void)
{
  PORT->rCTLW0.r = UCSWRST;
  PORT->rCTLW0.b.bSSEL = 2; // SMCLK

  float n = CS_getSMCLK() / 115200;
  uint16_t div;
  uint16_t frac;

  div = n / 16;
  frac = ((n / 16) - div) * 16;

  PORT->rBRW = div;
  PORT->rMCTLW.b.bBRF = frac;
  PORT->rMCTLW.b.bOS16 = 1;

  PORT->rCTLW0.b.bSWRST = 0;

  // Console shouldn't be realtime-critical,
  // use low interrupt priority for it.
  NVIC_ClearPendingIRQ(EUSCIA0_IRQn);
  NVIC_SetPriority(EUSCIA0_IRQn, PORT_CON_PRI);
  NVIC_EnableIRQ(EUSCIA0_IRQn);

#if NOSCFG_FEATURE_CONIN == 1

  PORT->rIE.b.bRXIE = 1; // Enable RX interrupt.

#endif
}

/*
 * Usart interrupt handler.
 */

void EUSCIA0_Handler()
{
  c_pos_intEnter();


  switch (PORT->rIV) {
  case 2:
#if NOSCFG_FEATURE_CONIN == 1

    {
      uint8_t ch;
      ch = PORT->rRXBUF.b.bRXBUF;
      c_nos_keyinput(ch);
    }

#endif
   break;

  case 4:
    PORT->rIE.b.bTXIE = 0; // Disable TX interrupt
#if NOSCFG_CONOUT_HANDSHAKE == 1 && NOSCFG_FEATURE_CONOUT == 1
    c_nos_putcharReady();
#endif
    break;

  default:
    break;
  }

  c_pos_intExitQuick();
}

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
#if NOSCFG_CONOUT_HANDSHAKE != 0

  if (PORT->rIE.b.bTXIE) // If interrupt is enabled, previous char is not yet transmitted.
    return 0;

  PORT->rTXBUF.b.bTXBUF = c;
  PORT->rIE.b.bTXIE = 1; // Enable TX interrupt

#else

  while (!(PORT->rIFG.b.bTXIFG)); // Wait for TX buffer to empty
  PORT->rTXBUF.b.bTXBUF = c;

#endif

  return 1;
}
#endif

#endif
#endif
