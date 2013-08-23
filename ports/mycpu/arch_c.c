/*
 *  Copyright (c) 2004-2012, Dennis Kuschel.
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


/**
 * @file    arch_c.c
 * @brief   pico]OS port for MyCPU (http://mycpu.eu)
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: arch_c.c,v 1.3 2009/01/10 16:26:15 dkuschel Exp $
 */


#include <time.h>
#include <stdlib.h>
#include <mycpu.h>

#define NANOINTERNAL
#include <picoos.h>



/*-------------------------------------------------------------------------
 *  DEFINITIONS
 *-----------------------------------------------------------------------*/

#define DSTACK_SIZE_MAX    0x400    /* 1.0  KB */
#define DSTACK_SIZE_MIN    0x300    /* 0.75 KB */
#define DSTACK_SIZE_IDLE   0x200    /* 0.5  KB */



/*-------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *-----------------------------------------------------------------------*/

unsigned char     inInterruptFlag_g = 0;
unsigned char     numberOfTasks_g   = 0;
static POSTASK_t  rootTask_g        = NULL;
static char*      rootDataStack_g   = NULL;



/*---------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES OF IMPORTED FUNCTIONS
 *-------------------------------------------------------------------------*/

extern unsigned char __fastcall__ setupStack(
                          unsigned char page,
                          unsigned char zeropage,
                          void POSCALL (*exitfunc)(void),
                          POSTASKFUNC_t funcptr, void *funcarg);

extern unsigned char __fastcall__ setupZeropage(
                          unsigned char page, void *datastack );

extern void  __fastcall__  saveContext(void);
extern void  __fastcall__  dropZSPage( unsigned char page );
extern void  __fastcall__  useZSPage( unsigned char page );
extern void  __fastcall__  freeAllZSPages( void );
extern void  __fastcall__  pPrintErr( char *s );
extern char* __fastcall__  getDataStackPtr( void );
extern void  __fastcall__  testForCTRLC( void );



/*---------------------------------------------------------------------------
 *  LOCAL FUNCTION PROTOTYPES
 *-------------------------------------------------------------------------*/

void POSCALL p_pos_sleep(unsigned short sec);
void POSCALL p_pos_semaGet(POSSEMA_t sem);
void timerInterruptHandler(void);



/*-------------------------------------------------------------------------
 *  TIMER INTERRUPT
 *-----------------------------------------------------------------------*/

void timerInterruptHandler(void)
{
  static unsigned char lastclk = 0;
  unsigned char clk;

  inInterruptFlag_g = 1;
  clk = (unsigned char) clock();
  if (lastclk != clk)
  {
    lastclk = clk;
    saveContext();
    c_pos_intEnter();
    c_pos_timerInterrupt();
    c_pos_intExit();
  }
  else
  {
    testForCTRLC();
  }
  inInterruptFlag_g = 0;
}



/*---------------------------------------------------------------------------
 * INIT TASK CONTEXT
 *-------------------------------------------------------------------------*/

#if (POSCFG_TASKSTACKTYPE != 2)
#error Only  POSCFG_TASKSTACKTYPE = 2  supported
#endif

VAR_t POSCALL p_pos_initTask(POSTASK_t task,
                             POSTASKFUNC_t funcptr, void *funcarg)
{
  unsigned char zp, sp;
  unsigned int size;
  char *ds, *eds;

  _dint();

  size = DSTACK_SIZE_MAX;
  if (numberOfTasks_g == 0)  size = DSTACK_SIZE_IDLE; else
  if (numberOfTasks_g >  6)  size = DSTACK_SIZE_MIN;

  if ((numberOfTasks_g == 1) && (rootTask_g == NULL))
  {
    rootTask_g = task;
    rootDataStack_g = getDataStackPtr() - 64;
  }
  if (task == rootTask_g)
  {
    ds  = NULL;
    eds = rootDataStack_g;
  }
  else
  {
    ds = (char*) malloc(size);
    if (ds == NULL)
      return -1;
    eds = ds + size - 2;
  }
  zp = allocZeroStackPage();
  if (zp == 0)
    goto freeds;
  sp = allocZeroStackPage();
  if (sp == 0)
    goto freezp;

  useZSPage(zp);
  useZSPage(sp);

  task->zeropage  = zp;
  task->stackpage = sp;
  task->datastack = ds;
  task->serrno    = 0;

#if POSCFG_FEATURE_EXIT != 0
  task->stackptr = setupStack(sp, zp, posTaskExit, funcptr, funcarg);
#else
  task->stackptr = setupStack(sp, zp, exit, funcptr, funcarg);
#endif

  setupZeropage(zp, eds);
  *((int*)eds) = (int)funcarg;

  numberOfTasks_g++;
  _eint();
  return 0;
  
freezp:
  freeZeroStackPage(zp);
freeds:
  free(ds);
  _eint();
  if (!posRunning_g)
  {
    pPrintErr("NO MEM EXTENSION\n");
    exit(1);
  }
  return -1;
}


void POSCALL p_pos_freeStack(POSTASK_t task)
{
  _dint();
  dropZSPage(task->zeropage);
  dropZSPage(task->stackpage);
  freeZeroStackPage(task->zeropage);
  freeZeroStackPage(task->stackpage);
  free(task->datastack);
  numberOfTasks_g--;
  _eint();
}



/*---------------------------------------------------------------------------
 * MISC. FUNCTIONS
 *-------------------------------------------------------------------------*/


void POSCALL p_pos_semaGet(POSSEMA_t sem)
{
  posSemaGet(sem);
}


void POSCALL p_pos_sleep(unsigned short sec)
{
  while (sec != 0)
  {
    posTaskSleep(HZ);
    sec--;
  }
}
