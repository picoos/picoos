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

/*
 * Initialize NXP/LPC uart console.
 */

#ifdef PORTCFG_CON_LPC

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
void portInitConsole(void)
{
  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaud(LPC_USART, PORTCFG_CONSOLE_SPEED);
  Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);

#if 0
  /* Enable receive data and line status interrupt */
  Chip_UART_IntEnable(LPC_USART, (UART_IER_RBRINT | UART_IER_RLSINT));

  /* preemption = 1, sub-priority = 1 */
  NVIC_SetPriority(UART0_IRQn, 1);
  NVIC_EnableIRQ(UART0_IRQn);

#endif
}

/*
 * Uart interrupt handler.
 */

void Uart_Handler()
{
#if 0
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
#endif
}

#endif

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
  while ((Chip_UART_ReadLineStatus(LPC_USART) & UART_LSR_THRE) == 0);
  Chip_UART_SendByte(LPC_USART, c);
  return 1;
/*
  if (!(UART0_LSR & 0x20))
    return 0;

  UART0_THR = c;
  return 1;*/
}
#endif

#endif
