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
 * @file   n_reg.c
 * @brief  nano layer, registry implementation
 * @author Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: n_reg.c,v 1.5 2006/10/16 19:41:27 dkuschel Exp $
 */


#define _N_REG_C
#include "../src/nano/privnano.h"


#if NOSCFG_FEATURE_REGISTRY != 0

#if NOSCFG_FEATURE_SPRINTF == 0
#include <stdio.h>
#endif

#if POSCFG_FEATURE_SEMAPHORES == 0
#error POSCFG_FEATURE_SEMAPHORES not enabled
#endif
#if NOSCFG_FEATURE_MEMALLOC == 0
#error NOSCFG_FEATURE_MEMALLOC not enabled
#endif


/*---------------------------------------------------------------------------
 * CONFIGURATION
 *-------------------------------------------------------------------------*/

#define KEY_MAXNAMENBR  9999



/*---------------------------------------------------------------------------
 *  TYPEDEFS AND VARIABLES
 *-------------------------------------------------------------------------*/

typedef struct regquery {
  REGELEM_t     queryElem;
  NOSREGTYPE_t  type;
} *REGQUERY_t;



/*---------------------------------------------------------------------------
 * GLOBAL VARIABLES
 *-------------------------------------------------------------------------*/

static REGELEM_t  reglist_free_g;
static POSSEMA_t  reglist_sema_g;
static REGELEM_t  reglist_syselem_g[MAX_REGTYPE+1];



/*---------------------------------------------------------------------------
 * MACROS
 *-------------------------------------------------------------------------*/

#if POSCFG_ALIGNMENT < 2
#define ALIGNEDSIZE(size)       (size)
#define NEXTALIGNED(type, var)  (((type)((void*)(var))) + 1)
#else
#define ALIGNEDSIZE(size) \
  (((size) + (POSCFG_ALIGNMENT - 1)) & ~(POSCFG_ALIGNMENT - 1))
#define NEXTALIGNED(type, var) \
  (type)((void*)(((MEMPTR_t)(var)) + ALIGNEDSIZE(sizeof(*(var)))))
#endif

#define REEUNKNOWN          ((REGELEM_t)((void*)-1))
#if NOSCFG_FEATURE_REGQUERY
#define HAVE_REGREFCOUNT
#define INC_REFCOUNT(re)    ++re->refcount
#define DEC_REFCOUNT(re, type)  n_remove(re, REEUNKNOWN, type)
#define IS_REFERENCED(re)   (re->refcount > 0)
#define RESET_REFCOUNT(re)  re->refcount = 0
#else
#define RESET_REFCOUNT(re)  do{}while(0)
#endif

#define IS_VISIBLE(re)      (re->state == 1)
#define IS_DELETED(re)      (re->state == 2)
#define MARK_INVISIBLE(re)  re->state = 0;
#define MARK_VISIBLE(re)    re->state = 1;
#define MARK_DELETED(re)    re->state = 2;



/*---------------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 *-------------------------------------------------------------------------*/

/* exports */
void      POSCALL nos_initRegistry(void);
REGELEM_t POSCALL nos_regNewSysKey(NOSREGTYPE_t type, const char* name);
void      POSCALL nos_regEnableSysKey(REGELEM_t re, NOSGENERICHANDLE_t handle);
void      POSCALL nos_regDelSysKey(NOSREGTYPE_t type,
                                   NOSGENERICHANDLE_t handle, REGELEM_t delre);

/* imports */

/* private */
static VAR_t     POSCALL n_strlen(const char *buf);
static REGELEM_t POSCALL n_regAlloc(void);
static void      POSCALL n_regFree(REGELEM_t re);
static void      POSCALL n_remove(REGELEM_t re, REGELEM_t rl,
                                  NOSREGTYPE_t type);
static REGELEM_t POSCALL n_findKeyByName(NOSREGTYPE_t type,
                                         const char *keyname);
static REGELEM_t POSCALL n_findKeyByHandle(NOSREGTYPE_t type,
                                           NOSGENERICHANDLE_t handle);
static void      POSCALL n_buildKeyName(char *dest, const char *basename,
                                        VAR_t basenlen, INT_t nbr);
static VAR_t     POSCALL n_newKey(NOSREGTYPE_t type, 
                                  const char* name, REGELEM_t *reret);



/*---------------------------------------------------------------------------
 * IMPLEMENTATION OF FUNCTIONS
 *-------------------------------------------------------------------------*/


static VAR_t POSCALL n_strlen(const char *buf)
{
  VAR_t l;
  for (l=0; buf[l]!=0; ++l);
  return l;
}


static REGELEM_t POSCALL n_regAlloc(void)
{
  REGELEM_t re;
#if NOS_REGKEY_PREALLOC > 1
  UVAR_t i;
#endif

  re = reglist_free_g;
  if (re != NULL)
  {
    reglist_free_g = re->next;
  }
  else
  {
    re = (REGELEM_t) nosMemAlloc( NOS_REGKEY_PREALLOC *
                                  ALIGNEDSIZE(sizeof(struct regelem)) );
    if (re == NULL)
      return NULL;

#if NOS_REGKEY_PREALLOC > 1
    for (i=0; i<NOS_REGKEY_PREALLOC-1; ++i)
    {
      re->next = reglist_free_g;
      reglist_free_g = re;
      re = NEXTALIGNED(REGELEM_t, re);
    }
#endif
  }
  RESET_REFCOUNT(re);
  MARK_INVISIBLE(re);
  re->handle.generic = NULL;
#ifdef POS_DEBUGHELP
  re->name[NOS_MAX_REGKEYLEN] = 0;
#endif
  return re;
}


static void POSCALL n_regFree(REGELEM_t re)
{
  re->next = reglist_free_g;
  reglist_free_g = re;
}


static void POSCALL n_remove(REGELEM_t re, REGELEM_t rl, NOSREGTYPE_t type)
{
#ifdef HAVE_REGREFCOUNT
  if (IS_REFERENCED(re))
  {
    --re->refcount;
  }
  else
#endif
  {
    if (rl == REEUNKNOWN)
    {
      if (re == reglist_syselem_g[type])
      {
        rl = NULL;
      }
      else
      {
        for (rl = reglist_syselem_g[type];
             (rl != NULL) && (rl->next != re);
             rl = rl->next);

        if (rl == NULL) 
        {
          rl = REEUNKNOWN;
        }
      }
    }

    if (rl != REEUNKNOWN)
    {
      if (rl == NULL) {
        reglist_syselem_g[type] = re->next;
      } else {
        rl->next = re->next;
      }
    }
    n_regFree(re);
  }
}


/*-------------------------------------------------------------------------*/


static REGELEM_t POSCALL n_findKeyByName(NOSREGTYPE_t type,
                                         const char *keyname)
{
  REGELEM_t re;
  VAR_t i;

  for (re = reglist_syselem_g[type]; re != NULL; re = re->next)
  {
    if (!IS_DELETED(re))
    {
      for (i=0; i<NOS_MAX_REGKEYLEN; ++i)
      {
        if (re->name[i] != keyname[i])
          break;
        if ((keyname[i] == 0) || (i == NOS_MAX_REGKEYLEN-1))
          return re;
      }
    }
  }
  return NULL;
}


static REGELEM_t POSCALL n_findKeyByHandle(NOSREGTYPE_t type,
                                           NOSGENERICHANDLE_t handle)

{
  REGELEM_t re;

  for (re = reglist_syselem_g[type]; re != NULL; re = re->next)
  {
    if ((re->handle.generic == handle) && !IS_DELETED(re))
      break;
  }
  return re;
}


static void POSCALL n_buildKeyName(char *dest, const char *basename,
                                   VAR_t baselen, INT_t nbr)
{
  char buf[5];
  VAR_t l, i;

#if NOSCFG_FEATURE_SPRINTF != 0
  nosSPrintf1(buf, "%i", nbr);
#else
  sprintf(buf, "%i", nbr);
#endif

  l = n_strlen(buf);

  if ((baselen + l) >= NOS_MAX_REGKEYLEN) {
    baselen = NOS_MAX_REGKEYLEN - l;
  } else {
    dest[baselen + l] = 0;
  }

  for (i=0; i<baselen; ++i)
    dest[i] = basename[i]; 

  for (i=0; i<l; ++i)
    dest[baselen + i] = buf[i];
}


static VAR_t POSCALL n_newKey(NOSREGTYPE_t type,
                              const char* name, REGELEM_t *reret)
{
  REGELEM_t re;
#if NOSCFG_FEATURE_REGQUERY != 0
  REGELEM_t ri;
#endif
  VAR_t i, bl, status;
  INT_t n;

  bl = n_strlen(name);
  if (bl == 0)
    return -E_FAIL;

  re = n_regAlloc();
  if (re == NULL)
    return -E_NOMEM;

  status = -E_FAIL;
  if ((bl > NOS_MAX_REGKEYLEN) || (name[bl - 1] == '*'))
  {
    if (bl > NOS_MAX_REGKEYLEN)
      bl = NOS_MAX_REGKEYLEN;
    --bl;
    for (n=0; n <= KEY_MAXNAMENBR; ++n)
    {
      n_buildKeyName(re->name, name, bl, n);
      if (n_findKeyByName(type, re->name) == NULL)
      {
        status = E_OK;
        break;
      }
    }
  }
  else
  {
    if (n_findKeyByName(type, name) == NULL)
    {
      status = E_OK;
      for (i=0; i<bl; ++i)
        re->name[i] = name[i];
      if (i < NOS_MAX_REGKEYLEN)
        re->name[i] = 0;
    }
  }

  if (status != E_OK)
  {
    n_regFree(re);
  }
  else
  {
#if NOSCFG_FEATURE_REGQUERY != 0
    re->next = NULL;
    ri = reglist_syselem_g[type];
    if (ri == NULL)
    {
      reglist_syselem_g[type] = re;
    }
    else
    {
      for (; ri->next != NULL; ri = ri->next);
      ri->next = re;
    }
#else
    re->next = reglist_syselem_g[type];
    reglist_syselem_g[type] = re;
#endif
    *reret = re;
  }

  return status;
}


/*-------------------------------------------------------------------------*/


#if NOSCFG_FEATURE_USERREG != 0

VAR_t POSCALL nosRegGet(const char *keyname, KEYVALUE_t *keyvalue)
{
  REGELEM_t re;

  if ((keyname == NULL) || (keyvalue == NULL))
    return -E_ARG;

  posSemaGet(reglist_sema_g);

  re = n_findKeyByName(REGTYPE_USER, keyname);
  if (re != NULL)
  {
    if (IS_VISIBLE(re)) {
      *keyvalue = re->handle.ukv;
    } else {
      re = NULL;
    }
  }

  posSemaSignal(reglist_sema_g);
  return (re == NULL) ? -E_FAIL : E_OK;
}


VAR_t POSCALL nosRegSet(const char *keyname, KEYVALUE_t keyvalue)
{
  REGELEM_t re;
  VAR_t  status = E_OK;

  if (keyname == NULL)
    return -E_ARG;

  posSemaGet(reglist_sema_g);

  re = n_findKeyByName(REGTYPE_USER, keyname);
  if (re == NULL)
  {
    status = n_newKey(REGTYPE_USER, keyname, &re);
  }
  if (status == E_OK)
  {
    re->handle.ukv = keyvalue;
    MARK_VISIBLE(re);
  }

  posSemaSignal(reglist_sema_g);
  return status;
}


VAR_t POSCALL nosRegDel(const char *keyname)
{
  REGELEM_t re, rl;
  VAR_t i;

  posSemaGet(reglist_sema_g);

  for (re = reglist_syselem_g[REGTYPE_USER], rl = NULL;
       re != NULL; rl = re, re = re->next)
  {
    if (!IS_DELETED(re))
    {
      for (i=0; i<NOS_MAX_REGKEYLEN; ++i)
      {
        if (re->name[i] != keyname[i])
          break;
        if ((keyname[i] == 0) || (i == NOS_MAX_REGKEYLEN-1))
        {
          MARK_DELETED(re);
          n_remove(re, rl, REGTYPE_USER);
          posSemaSignal(reglist_sema_g);
          return E_OK;
        }
      }
    }
  }

  posSemaSignal(reglist_sema_g);
  return -E_FAIL;
}

#endif /* NOSCFG_FEATURE_USERREG */


/*-------------------------------------------------------------------------*/


#if NOSCFG_FEATURE_REGQUERY != 0

NOSREGQHANDLE_t POSCALL nosRegQueryBegin(NOSREGTYPE_t type)
{
  REGQUERY_t  rq;

  if (type > MAX_REGTYPE)
    return NULL;

  rq = (REGQUERY_t) nosMemAlloc(sizeof(struct regquery));
  if (rq != NULL)
  {
    rq->queryElem = NULL;
    rq->type = type;
  }
  return (NOSREGQHANDLE_t) rq;
}


VAR_t POSCALL nosRegQueryElem(NOSREGQHANDLE_t qh, NOSGENERICHANDLE_t *genh,
                              char* namebuf, VAR_t bufsize)
{
  REGQUERY_t rq = (REGQUERY_t) qh;
  REGELEM_t re;
  VAR_t i;

  if ((rq == NULL) || (genh == NULL) || (namebuf == NULL))
    return -E_ARG;

  posSemaGet(reglist_sema_g);

  if (rq->queryElem == NULL)
  {
    re = reglist_syselem_g[rq->type];
  }
  else
  {
    re = rq->queryElem->next;
    DEC_REFCOUNT(rq->queryElem, rq->type);
  }

  for(;;)
  {
    rq->queryElem = re;

    if (re == NULL)
    {
      posSemaSignal(reglist_sema_g);
      return -E_NOMORE;
    }

    if (IS_VISIBLE(re))
      break;

    re = re->next;
  }

  INC_REFCOUNT(re);

  *genh = re->handle.generic;
  for (i=0; (i < NOS_MAX_REGKEYLEN) && 
            (i < (bufsize-1)) && (re->name[i] != 0); ++i)
  {
    namebuf[i] = re->name[i];
  }
  namebuf[i] = 0;

  posSemaSignal(reglist_sema_g);
  return E_OK;
}


void POSCALL nosRegQueryEnd(NOSREGQHANDLE_t qh)
{
  REGQUERY_t rq = (REGQUERY_t) qh;

  if (rq != NULL)
  {
    if (rq->queryElem != NULL)
    {
      posSemaGet(reglist_sema_g);
      DEC_REFCOUNT(rq->queryElem, rq->type);
      posSemaSignal(reglist_sema_g);
    }
    nosMemFree(rq);
  }
}

#endif /* NOSCFG_FEATURE_REGQUERY */


/*-------------------------------------------------------------------------*/


NOSGENERICHANDLE_t POSCALL nosGetHandleByName(NOSREGTYPE_t objtype, 
                                              const char *objname)
{
  REGELEM_t re;

  if ((objtype > MAX_REGTYPE) || (objname ==NULL))
    return NULL;
  if (n_strlen(objname) > NOS_MAX_REGKEYLEN)
    return NULL;

  posSemaGet(reglist_sema_g);
  re = n_findKeyByName(objtype, objname);
  posSemaSignal(reglist_sema_g);

  if (re == NULL)
    return NULL;
  return IS_VISIBLE(re) ? re->handle.generic : NULL;
}


VAR_t POSCALL nosGetNameByHandle(NOSGENERICHANDLE_t handle,
                                 char *buffer, VAR_t bufsize,
                                 NOSREGTYPE_t what)
{
  NOSREGTYPE_t  rt;
  REGELEM_t re = NULL;
  VAR_t status = -E_NOTFOUND;
  VAR_t i;
  char  c;

  posSemaGet(reglist_sema_g);

  if (what != REGTYPE_SEARCHALL)
  {
    if (what <= MAX_REGTYPE)
    {
      re = n_findKeyByHandle(what, handle);
    }
    else
    {
      status = -E_ARG;
    }
  }
  else  
  for (rt = MIN_REGTYPE; rt <= MAX_REGTYPE; ++rt)
  {
#if NOSCFG_FEATURE_USERREG
    if (rt != REGTYPE_USER)
#endif
    {
      re = n_findKeyByHandle(rt, handle);
      if (re != NULL)
        break;
    }
  }

  if (re != NULL)
  {
    if (IS_VISIBLE(re))
    {
      status = E_OK;
      if ((buffer != NULL) && (bufsize > 0))
      {
        for (i=0; (i<bufsize-1) && (i<NOS_MAX_REGKEYLEN); ++i)
        {
          c = re->name[i];
          if (c == 0)  break;
          buffer[i] = c;
        }
        buffer[i] = 0;
      }
    }
  }

  posSemaSignal(reglist_sema_g);
  return status;
}


/*-------------------------------------------------------------------------*/


void POSCALL nos_regDelSysKey(NOSREGTYPE_t type, NOSGENERICHANDLE_t handle,
                              REGELEM_t re)
{
  REGELEM_t rl = REEUNKNOWN;

  posSemaGet(reglist_sema_g);
  if (re == NULL)
  {
    for (re = reglist_syselem_g[type], rl = NULL;
         re != NULL; rl = re, re = re->next)
    {
      if (!IS_DELETED(re) && (re->handle.generic == handle))
        break;
    }
  }
  if (re != NULL)
  {
    if (!IS_DELETED(re))
    {
      MARK_DELETED(re);
      n_remove(re, rl, type);
    }
  }
  posSemaSignal(reglist_sema_g);
}


REGELEM_t POSCALL nos_regNewSysKey(NOSREGTYPE_t type, const char* name)
{
  REGELEM_t re = NULL;
  posSemaGet(reglist_sema_g);
  (void) n_newKey(type, name, &re);
  posSemaSignal(reglist_sema_g);
  return re;
}


void POSCALL nos_regEnableSysKey(REGELEM_t re, NOSGENERICHANDLE_t handle)
{
  re->handle.generic = handle;
  MARK_VISIBLE(re);
}


void POSCALL nos_initRegistry(void)
{
  NOSREGTYPE_t  rt;

  reglist_free_g = NULL;
  reglist_sema_g = posSemaCreate(1);
  while (reglist_sema_g == NULL);
  POS_SETEVENTNAME(reglist_sema_g, "registry sync");

  for (rt = MIN_REGTYPE; rt <= MAX_REGTYPE; ++rt)
    reglist_syselem_g[rt] = NULL;
}

#else /* NOSCFG_FEATURE_REGISTRY */

void POSCALL nos_initRegistry(void);
void POSCALL nos_initRegistry(void)
{
}

#endif /* NOSCFG_FEATURE_REGISTRY */



