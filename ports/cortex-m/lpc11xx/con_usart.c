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

#if PORTCFG_CON_USART == 1

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
void portInitConsole(void)
{
  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaud(LPC_USART, PORTCFG_CONSOLE_SPEED);
  Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);

#if NOSCFG_FEATURE_CONIN == 1

  // Enable receive data interrupt.
  Chip_UART_IntEnable(LPC_USART, UART_IER_RBRINT);

#endif

  Chip_UART_IntEnable(LPC_USART, UART_IER_THREINT);

  // Console shouldn't be realtime-critical,
  // use low interrupt priority for it.
  NVIC_SetPriority(UART0_IRQn, PORT_CON_PRI);
  NVIC_EnableIRQ(UART0_IRQn);
}

/*
 * Uart interrupt handler.
 */

void Uart_Handler()
{
  c_pos_intEnter();
  uint32_t status;

  status = Chip_UART_ReadIntIDReg(LPC_USART);

#if NOSCFG_FEATURE_CONOUT == 1
  if (status & UART_IIR_INTID_THRE) {

    c_nos_putcharReady();
  }
#endif

#if NOSCFG_FEATURE_CONIN == 1
  if (status & (UART_IIR_INTID_RDA | UART_IIR_INTID_CTI)) {

    unsigned char ch;

    while (Chip_UART_ReadLineStatus(LPC_USART) & UART_LSR_RDR) {

      ch = Chip_UART_ReadByte(LPC_USART);

      c_nos_keyinput(ch);
    }
  }
#endif

  c_pos_intExitQuick();
}

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
  if ((Chip_UART_ReadLineStatus(LPC_USART) & UART_LSR_THRE) == 0)
    return 0;

  Chip_UART_SendByte(LPC_USART, c);
  return 1;
}
#endif

#endif
#endif
