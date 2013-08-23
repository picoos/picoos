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
 * This example shows the usage of a mutex. For a correct run the EXIT_FEATURE
 * has to be activated in poscfg.h.
 *
 *
 * CVS-ID $Id: mutex.c,v 1.2 2008/08/22 21:41:09 smocz Exp $
 */
#include <stdio.h>
#include <inttypes.h>

#include <avr/io.h>

#include "picoos.h"


static void initialTask(void* arg);
static void workerTask(void *arg);

static POSMUTEX_t    mutex;


static void workerTask(void *arg) {

    // this static variable will be used by all threads
    static uint_8 counter = 0;

    for (;;) {
        posMutexLock(mutex);    // try to get the mutex

        counter++;
        posTaskSleep(HZ/6);     // sleep one second
        counter--;

        if (counter != 0) {
            // print an error
        }

        posMutexUnlock(mutex);  // unlock mutex again
    }
}


static void initialTask(void *arg) {

    mutex = posMutexAlloc();

    if (mutex != NULL) {
        posTaskCreate(workerTask, NULL, 1);
        posTaskCreate(workerTask, NULL, 1);
        posTaskCreate(workerTask, NULL, 1);
    } else {
        // print an error
    }

}


int main(void) {

    posInit(initialTask, NULL, 2);

    return 0;
}
