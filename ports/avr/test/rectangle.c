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
 * This file is a simple test program to validate if pico]OS will work.
 *
 * The program starts two threads. Each thread write a different value
 * to the port pin PB0. The scheduler switch between the two tasks.
 * With a oscilloscope you should see a rectangle signal on PB0. The
 * frequency is 2 x HZ (see picocfg.h). The rectangle is a little bit
 * asymmetric, because the idle task also work in the background.
 *
 * --- build ---
 *
 * 1. Set in the makefile in directory $(picoos-root)/ports/avr/test
 *    SRC_TXT = rectangle.c
 *
 * 2. You can build the test by navigate to $(picoos-root)/ports/avr/test
 *    directory an type "make".
 *
 * 3. Now, in the directory $(picoos-root)/out/avr/deb should be a .elf,
 *    .cof and a .hex file.
 *
 * 4. You can open the .cof file with AvrStudio and watch, how pico]OS
 *    work.
 *    With the hex-file you can program the target device and run the
 *    program in the real world :-)
 *
 * CVS-ID $Id: rectangle.c,v 1.3 2008/08/22 21:41:09 smocz Exp $
 */
#include <stdio.h>
#include <inttypes.h>

#include <avr/io.h>

#include "picoos.h"


static void initialTask(void* arg);
static void pollTask(void *arg);

/* the initial values for the 2 tasks */
static uint8_t t1 = 0x01;
static uint8_t t2 = 0x00;

/* The start task for the initialization of the 2 other tasks.
 * This task will be destroyed, when the program runs out of the
 * block
 */
static void initialTask(void *arg) {

    posTaskCreate(pollTask, &t1, 1);
    posTaskCreate(pollTask, &t2, 1);
}


static void pollTask(void *arg) {
    uint8_t *valuePtr = (uint8_t*) arg;
    uint8_t value = *valuePtr;

    while(1) {
        PORTB = value;
    }
}

int main(void) {

    DDRB = 0x01;

    posInit(initialTask, NULL, 2);

    return 0;
}
