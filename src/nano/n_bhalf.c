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
 * @file   n_bhalf.c
 * @brief  nano layer, bottom half implementation
 * @author Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: n_bhalf.c,v 1.7 2006/10/16 19:41:27 dkuschel Exp $
 */

#define _N_BHALF_C
#include "../src/nano/privnano.h"

#if NOSCFG_FEATURE_BOTTOMHALF != 0

/* check features */
#if POSCFG_FEATURE_INHIBITSCHED == 0
#error POSCFG_FEATURE_INHIBITSCHED not enabled
#endif
#if POSCFG_FEATURE_SEMAPHORES == 0
#error POSCFG_FEATURE_SEMAPHORES not enabled
#endif
#if NOSCFG_FEATURE_TASKCREATE == 0
#error NOSCFG_FEATURE_TASKCREATE not enabled
#endif
#if POSCFG_FEATURE_SOFTINTS == 0
#error POSCFG_FEATURE_SOFTINTS not enabled
#endif



/*---------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *-------------------------------------------------------------------------*/

static struct {
  NOSBHFUNC_t  func;
  void         *arg;
} bottomhalf_g[NOS_MAX_BOTTOMHALFS];

static POSSEMA_t    bhsema_g;
static UVAR_t       bhexecmask_g;



/*---------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES
 *-------------------------------------------------------------------------*/

/* exported */
void POSCALL nos_initBottomHalfs(void);

/* private */
static void nos_bhtask(void *arg);
static void nos_bhtrigger(UVAR_t arg);



/*---------------------------------------------------------------------------
 *  BOTTOM HALF WORKER TASK
 *-------------------------------------------------------------------------*/

static void nos_bhtask(void *arg)
{
  UVAR_t bhm, i;
  POS_LOCKFLAGS;

  (void) arg;

  POS_SETTASKNAME(posCurrentTask_g, "bottomhalf task");

  for (;;)
  {
    (void) posSemaGet(bhsema_g);
    posTaskSchedLock();
    POS_SCHED_LOCK;
    bhm = bhexecmask_g;
    bhexecmask_g = 0;
    POS_SCHED_UNLOCK;

    i = 0;
    if (bhm != 0)
    {
      do
      {
        if ((bhm & 1) != 0)
        {
          if (bottomhalf_g[i].func != NULL)
          {
            (bottomhalf_g[i].func)(bottomhalf_g[i].arg, i);
          }
        }
        i++;
        bhm >>= 1;
      }
      while (bhm != 0);
    }

    posTaskSchedUnlock(); 
  }
}


static void nos_bhtrigger(UVAR_t arg)
{
  (void) arg;
  posSemaSignal(bhsema_g);
}



/*---------------------------------------------------------------------------
 *  BOTTOM HALF MANAGEMENT
 *-------------------------------------------------------------------------*/


void POSCALL nosBottomHalfStart(UVAR_t number)
{
  UVAR_t m;
  POS_LOCKFLAGS;

  if (number < NOS_MAX_BOTTOMHALFS)
  {
    POS_SCHED_LOCK;
    m = bhexecmask_g;
    bhexecmask_g |= pos_shift1l(number);
    POS_SCHED_UNLOCK;

    if (m == 0)
    {
      if (posInInterrupt_g == 0)
      {
        /* We are possibly not in a pico]OS interrupt context,
           so it is safer to rise a software interrupt. This
           will call nos_bhtrigger() and signal the semaphore. */
        posSoftInt(1, 0);
      }
      else
      {
        posSemaSignal(bhsema_g);
      }
    }
  }
}

/*-------------------------------------------------------------------------*/

VAR_t POSCALL nosBottomHalfRegister(UVAR_t number,
                                    NOSBHFUNC_t func, void *arg)
{
  POS_LOCKFLAGS;

  if (number >= NOS_MAX_BOTTOMHALFS)
    return -E_ARG;

  POS_SCHED_LOCK;
  if ((func != NULL) && (bottomhalf_g[number].func != NULL))
  {
    POS_SCHED_UNLOCK;
    return -E_FAIL;
  }
  bottomhalf_g[number].func = func;
  bottomhalf_g[number].arg  = arg;
  POS_SCHED_UNLOCK;
  return 0;
}

/*-------------------------------------------------------------------------*/

void POSCALL nos_initBottomHalfs(void)
{
  UVAR_t i;

  for (i=0; i<NOS_MAX_BOTTOMHALFS; i++)
  {
    bottomhalf_g[i].func = NULL;
  }
  bhexecmask_g  = 0;
  bhsema_g = posSemaCreate(0);
  POS_SETEVENTNAME(bhsema_g, "bottomhalf tasksync");
  (void) nosTaskCreate(nos_bhtask, NULL, POSCFG_MAX_PRIO_LEVEL - 1, 0, NULL);
  posSoftIntSetHandler(1, nos_bhtrigger);
}

#else  /* NOSCFG_FEATURE_BOTTOMHALF */

/* this is just a dummy function */
void POSCALL nos_initBottomHalfs(void);
void POSCALL nos_initBottomHalfs(void) {}

#endif /* NOSCFG_FEATURE_BOTTOMHALF */


