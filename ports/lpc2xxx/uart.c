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

static void Uart_Handler(void);

/*
 * Initialize uart console.
 */

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
void
portInitUart(void)
{
  unsigned short consoleDll;

/*
 * Initialize console. Our peripheral bus runs at same speed as
 * CPU.
 */

  consoleDll = (PORTCFG_CRYSTAL_CLOCK / (PORTCFG_CONSOLE_SPEED * 16.0)) + 0.5;
  UART0_LCR = UART_LCR_DLAB;
  UART0_DLL = (unsigned char)(consoleDll & 0x00ff);
  UART0_DLM = (unsigned char)(consoleDll >> 8);
  UART0_LCR = UART_LCR_NOPAR;

/*
 * 8 databits, no parity, 1 stopbit.
 */

  UART0_LCR = UART_LCR_8BITS;

/*
 * Enable/Reset FIFO.
 */

  UART0_FCR = UART_LCR_8BITS | UART_FCR_CLR;


/*
 * Configure & enable uart interrupts.
 */

  VIC_IntSelect &= ~(VIC_IntSelect_UART0);
  VIC_IntEnable |= VIC_IntEnable_UART0;
  VIC_VectAddr1 = (unsigned long)Uart_Handler;
  VIC_VectCntl1 = VIC_Channel_UART0 | VIC_VectCntl_ENABLE;
  
  UART0_IER = UART_IER_EI;
}

/*
 * Uart interrupt handler.
 */

static void
Uart_Handler()
{
  int reason;
  unsigned char ch;
		
  portSaveContext();
  c_pos_intEnter();

  reason = UART0_IIR & 0xf;
  switch (reason) {
  case 0x2: // TX
#if NOSCFG_FEATURE_CONOUT == 1
    c_nos_putcharReady();
#endif
    break;

  case 0x4: // RX
    ch = UART0_RBR;
#if NOSCFG_FEATURE_CONIN == 1
    c_nos_keyinput(ch);
#endif
    break;

  case 0x6: // err
    ch= UART0_LSR;
    break;
  }

  VIC_VectAddr = 0;
  c_pos_intExit();
  portRestoreContext();
}

#endif

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
  if (!(UART0_LSR & 0x20))
    return 0;

  UART0_THR = c;
  return 1;
}
#endif
