/*
 * Copyright (c) 2006, Ari Suutari, ari@suutari.iki.fi. All rights reserved.
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

#include "picoos.h"
#include "lpc_reg.h"

#ifdef BOARD_OLIMEX

#define LED_GREEN	(1 << 10)
#define	LED_RED		(1 << 8)
#define LED_YELLOW	(1 << 11)
#define	LED_IOSET	GPIO0_IOSET
#define	LED_IOCLR	GPIO0_IOCLR
#define	LED_IODIR	GPIO0_IODIR

#endif


#ifdef BOARD_EA

#define LED_GREEN	(1 << 24)
#define LED_YELLOW	(1 << 25)
#define	LED_RED		0
#define	LED_IOSET	GPIO1_IOSET
#define	LED_IOCLR	GPIO1_IOCLR
#define	LED_IODIR	GPIO1_IODIR

#endif


void		LedTask   (void *arg);
void		DelayTask (void *arg);
void		osMainTask(void *arg);

void 
LedTask(void *arg)
{
  int		  x = 0;
  while (1) {
    if (x % 2)
      LED_IOSET = LED_YELLOW;
    else
      LED_IOCLR = LED_YELLOW;

    posTaskSleep(200);
    ++x;
    if (x >= 10)
      x = 0;
  }
}

void 
DelayTask(void *arg)
{
  int		  x = 0;

  while (1) {
    if (x % 2)
      LED_IOSET = LED_GREEN;
    else
      LED_IOCLR = LED_GREEN;

    posTaskSleep(10000);
    nosPrintf1("delayTask woke up x=%d!\n", x);
    ++x;
    if (x >= 10)
      x = 0;
  }
}

void 
osMainTask(void *memstart)
{
  LED_IODIR = LED_RED | LED_GREEN | LED_YELLOW;
  int		  x = 20;

  nosPrint("mainTask start!\n");
  posTaskCreate(DelayTask, NULL, 5, 1000);
  posTaskCreate(LedTask, NULL, 15, 300);

  while (1) {
    if (x % 2)
      LED_IOSET = LED_RED;
    else
      LED_IOCLR = LED_RED;

    posTaskSleep(5000);
    nosPrintf1("mainTask woke up x=%d!\n", x);
    --x;
    if (x < 10)
      x = 20;
  }
}

int 
main(int argc, char **argv)
{
#ifdef POSNANO
  nosInit(osMainTask, NULL, 10, 1000, 1000);
#else
  posInit(osMainTask, NULL, 10,
	  1000, 1000);
#endif
  return 0;
}
