/*
 * Copyright (c) 2015, Ari Suutari <ari@stonepile.fi>.
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

#ifndef _PORT_IRQ_H
#define _PORT_IRQ_H

/*
 * Define IRQ priorities. Use defaults from port.h if
 * not overridden in poscfg.h
 */

#ifdef PORTCFG_API_MAX_PRI
#define PORT_API_MAX_PRI PORTCFG_API_MAX_PRI
#else
#define PORT_API_MAX_PRI PORT_DEFAULT_API_MAX_PRI
#endif

#ifdef PORTCFG_SVCALL_PRI
#define PORT_SVCALL_PRI PORTCFG_SVCALL_PRI
#else
#define PORT_SVCALL_PRI PORT_DEFAULT_SVCALL_PRI
#endif

#ifdef PORTCFG_SYSTICK_PRI
#define PORT_SYSTICK_PRI PORTCFG_SYSTICK_PRI
#else
#define PORT_SYSTICK_PRI PORT_DEFAULT_SYSTICK_PRI
#endif

#ifdef PORTCFG_PENDSV_PRI
#define PORT_PENDSV_PRI PORTCFG_PENDSV_PRI
#else
#define PORT_PENDSV_PRI PORT_DEFAULT_PENDSV_PRI
#endif

#ifdef PORTCFG_CON_PRI
#define PORT_CON_PRI PORTCFG_CON_PRI
#else
#define PORT_CON_PRI PORT_DEFAULT_CON_PRI
#endif

#endif /* _PORT_IRQ_H */
