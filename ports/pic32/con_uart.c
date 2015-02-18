/*
 * Copyright (c) 2014-2015, Ari Suutari <ari@stonepile.fi>.
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
 * Initialize UART console.
 */

#if PORTCFG_CONOUT_UART == 1

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
void portInitConsole(void)
{
  U1BRG = ((PORTCFG_CRYSTAL_CLOCK) /(1 << OSCCONbits.PBDIV))/16/38400-1; // Set Baud rate
  U2STA = 0;
  U2MODE = 0;

  U2MODEbits.UARTEN = 1;
  U2STAbits.UTXEN = 1;
  U2STAbits.URXEN = 1;

  IFS1bits.U2RXIF = 0;
  IFS1bits.U2TXIF = 0;
  IPC9bits.U2IP = 2;
  IPC9bits.U2IS = 0;
  IEC1bits.U2RXIE = 1;
  IEC1bits.U2TXIE = 1;
}

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
  if (U2STA & _U1STA_UTXBF_MASK)
    return 0;

  U2TXREG = c;
  return 1;
}
#endif

void  PORT_NAKED __attribute__((vector(_UART2_VECTOR))) Uart2Handler(void)
{
  portSaveContext();
  c_pos_intEnter();

  if (IFS1bits.U2RXIF) {

    IFS1CLR = _IFS1_U2RXIF_MASK;
  }

  if (IFS1bits.U2TXIF) {

    IFS1CLR = _IFS1_U2TXIF_MASK;
    c_nos_putcharReady();
  }

  c_pos_intExitQuick();
  portRestoreContext();
}

#endif
#endif
