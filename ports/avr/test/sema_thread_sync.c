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
 * This is a simple test program, that demonstrate the use of mutexes.
 * Three tasks read a global summe, wait and write then the summe + 1 
 * back. This critical block is protected by a mutex. 
 * If you comment the mutex out, at the end you will see, that summe 
 * has a wong value.
 * 
 * --- build ---
 * 
 * 1. Set in the makefile in directory $(picoos-root)/ports/avr/test
 *    SRC_TXT = mutexSumme.c
 * 
 * 2. You can then build the test by navigate to $(picoos-root)/ports/avr/test 
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
 * CVS-ID $Id: sema_thread_sync.c,v 1.1 2004/02/23 19:45:45 smocz Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <avr/io.h>

#include "picoos.h"


static void initialTask(void* arg);
static void workerTask(void *arg);

static POSMUTEX_t    mutex;

static POSSEMA_t     finishSema;


/* this static variable will be used by all threads */
static uint8_t summe = 0;


/*
 * This task will be start three times. Each task increments
 * ten times the global variable summe.
 */
static void workerTask(void *arg) {
    
    uint8_t i;
    
    for (i=0; i < 10; i++) {
      
        // get the mutex
        posMutexLock(mutex);

        uint8_t localSumme = summe;
        
        posTaskSleep(HZ/1000);
        
        localSumme++;
        summe = localSumme;

        // unlock mutex
        posMutexUnlock(mutex);
    }
    // signal, that this thread has been finished
    posSemaSignal(finishSema);
}

/*
 * The initial task start the other three worker tasks.
 * When all worker tasks have been finished, the resulting
 * summe will be controlled.
 */
static void initialTask(void *arg) {
  
  
    finishSema = posSemaAlloc(-2);
    
    if (finishSema == NULL) {
        // error, can't get the semaphore
        exit(1);
    }
  
    mutex = posMutexAlloc();
  
    if (mutex == NULL) {
        // error, can't get a mutex
        exit(1);
    }
    if (posTaskCreate(workerTask, NULL, 1) == NULL) {
        exit(1);
    }
    posTaskSleep(HZ/300);
    
    if (posTaskCreate(workerTask, NULL, 1) == NULL) {
        exit(1);
    }
    posTaskSleep(HZ/700);
    
    if (posTaskCreate(workerTask, NULL, 1) == NULL) {
     exit(1);
    }
    
    // wait until all tasks have been terminated
    posSemaGet(finishSema);

    // 3 tasks increment summe 10 times, so summe has to be 30
    if (summe == 30) {
        // success
        PORTA = summe;
    } else {
        // print error
    }
}


int main(void) {
    
    posInit(initialTask, NULL, 2);
  
    return 0;
}
