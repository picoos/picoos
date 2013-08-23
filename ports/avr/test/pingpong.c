/*
 *  Copyright (c) 2004, Swen Moczarski.
 *  All rights reserved. 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: pingpong.c,v 1.1.1.1 2004/02/16 20:11:31 smocz Exp $
 */

#include <stdio.h>
#include <inttypes.h>

#include <avr/io.h>

#include "picoos.h"


static void initialTask(void* arg);
static void pollTask(void *arg);

static uint8_t t1 = 0x01;
static uint8_t t2 = 0x00;

static void initialTask(void *arg) {
  
    // ATTENTION: I kill the timer interrupt only for test purpose
    TIMSK &= ~_BV(OCIE1A);
  
    posTaskCreate(pollTask, &t1, 2);
    // posTaskSleep(HZ/8);
    posTaskCreate(pollTask, &t2, 2);
}


static void pollTask(void *arg) {
    uint8_t *valuePtr = (uint8_t*) arg;
    uint8_t value = *valuePtr;

    for (;;) {
        PORTB = value;
        posTaskYield();
    }
}

int main(void) {
    
    DDRB = 0x01;
    
    posInit(initialTask, NULL, 1);
  
    return 0;
}
