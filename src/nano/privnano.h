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
 *  This file is part of the pico]OS realtime operating system.
 *
 *  $Id: privnano.h,v 1.4 2006/10/16 19:41:27 dkuschel Exp $
 *
 */

#ifndef _PRIVNANO_H
#define _PRIVNANO_H
#define NANOINTERNAL

#define PICOS_PRIVINCL
#include <pos_nano.h>

#if NOSCFG_MEM_MANAGER_TYPE == 0
#include <stdlib.h>
#endif

/*-------------------------------------------------------------------------*/

#if POSCFG_FEATURE_INHIBITSCHED == 0
#error POSCFG_FEATURE_INHIBITSCHED not enabled!
#endif

/*-------------------------------------------------------------------------*/

#if NOSCFG_FEATURE_REGISTRY != 0

union khandle {
  NOSGENERICHANDLE_t  generic;
  POSTASK_t   tsk;
  POSSEMA_t   sem;
#if POSCFG_FEATURE_MUTEXES != 0
  POSMUTEX_t  mtx;
#endif
#if POSCFG_FEATURE_FLAGS != 0
  POSFLAG_t   flg;
#endif
#if POSCFG_FEATURE_TIMER != 0
  POSTIMER_t  tmr;
#endif
#if NOSCFG_FEATURE_USERREG != 0
  KEYVALUE_t  ukv;
#endif
};

struct regelem;
struct regelem {
  struct regelem  *next;
  union khandle   handle;
  volatile UVAR_t state;
#if NOSCFG_FEATURE_REGQUERY
  volatile UVAR_t refcount;
#endif
#ifdef POS_DEBUGHELP
  char  name[NOS_MAX_REGKEYLEN+1];
#else
  char  name[NOS_MAX_REGKEYLEN];
#endif
};
typedef struct regelem* REGELEM_t;

#ifndef _N_REG_C

extern REGELEM_t POSCALL nos_regNewSysKey(NOSREGTYPE_t type,
                                          const char* name);
extern void POSCALL nos_regDelSysKey(NOSREGTYPE_t type,
                                     NOSGENERICHANDLE_t handle,
                                     REGELEM_t delre);
extern void  POSCALL nos_regEnableSysKey(REGELEM_t re,
                                         NOSGENERICHANDLE_t handle);

#endif /* _N_REG_C */

#endif /* NOSCFG_FEATURE_REGISTRY */

/*-------------------------------------------------------------------------*/

#endif /* _PRIVNANO_H */
