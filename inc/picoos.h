/*
 * License:  Modified BSD license, see documentation below.
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: picoos.h,v 1.29 2010/02/20 13:18:52 dkuschel Exp $
 *
 */

/**
 * @file    picoos.h
 * @brief   pico]OS main include file
 * @author  Dennis Kuschel
 */

/**
 * @mainpage pico]OS - Real Time Operating System
 *
 * <b> Table Of Contents </b>@n@htmlonly<font size="-1">@endhtmlonly
 * <ul><li>@ref overview <ul>
 *   <li> @ref features </li><li> @ref ports     </li>
 *   <li> @ref files    </li><li> @ref license   </li></ul></li></ul>
 * <ul><li> @ref intro <ul>
 *   <li> @ref layer    </li><li> @ref port      </li>
 *   <li> @ref debug    </li><li> @ref make      </li></ul></li></ul>
 * <ul><li><b>Configuration</b><ul>
 *   <li><b>Pico Layer</b><ul>
 *     <li> @ref arch     </li><li> @ref codestyle </li>
 *     <li> @ref lock     </li><li> @ref feature   </li>
 *     <li> @ref findbit  </li><li> @ref portnlcfg </li>
 *     <li> @ref coreset  </li></ul></li></ul><ul>
 *   <li><b>Nano Layer</b><ul>
 *     <li> @ref cfgabstr </li><li> @ref cfgnosbh </li>
 *     <li> @ref cfgnoscio</li><li> @ref cfgcpuu  </li>
 *     <li> @ref cfgnosmem</li><li> @ref cfgnosreg</li></ul></li></ul>
 * </li></ul><ul><li><b>User API Function Reference</b><ul>
 *   <li><b>Pico Layer</b><ul>
 *     <li> @ref atomic   </li><li> @ref errcodes </li>
 *     <li> @ref flag     </li><li> @ref lists    </li>
 *     <li> @ref msg      </li><li> @ref mutex    </li>
 *     <li> @ref sema     </li><li> @ref sint     </li>
 *     <li> @ref task     </li><li> @ref timer    </li></ul></li>
 *   <li><b>Nano Layer</b><ul><li> @ref absfunc <ul>
 *       <li> @ref nanoflag </li><li> @ref nanomsg  </li>
 *       <li> @ref nanomutex</li><li> @ref nanosema </li>
 *       <li> @ref nanotask </li><li> @ref nanotimer</li></ul></li>
 *     <li> @ref bhalf    </li><li> @ref conio    </li>
 *     <li> @ref cpuusage </li><li> @ref nanoinit </li>
 *     <li> @ref mem      </li><li> @ref registry </li></ul></li></ul>
 * </li></ul>@htmlonly</font>@endhtmlonly@n<hr>@n
 * 
 * @section overview Overview
 * pico]OS is a highly configurable and very fast real time operating
 * system. It targets a wide range of architectures, from the small
 * 8 bit processors with very low memory till huge architectures
 * like 32 bit processors with lots of memory.@n@n@n
 *
 * @subsection features Features
 * pico]OS is currently divided into two layer. The <b>pico-layer</b>
 * (the core of the operating system) includes this features:
 *
 * <b>Scheduler:</b>
 *  - two modes: standard priority based or round robin
 *  - preemptive multitasking
 *  - max. 64 tasks on 8 bit processors,
 *    max. 1024 tasks on 32 bit processors
 *  - round robin: max. 8 priority level on 8 bit processors,
 *    max. 32 priority level on 32 bit processors
 *
 * <b>Events:</b>
 *  - the number of events is unlimited
 *  - semaphores
 *  - mutexes
 *  - flag events
 *
 * <b>Message Boxes:</b>
 *  - one built in message box per task
 *  - unlimited box size, messages are collected
 *  - messages can be pointers or huge buffers
 *
 * <b>Timer:</b>
 *  - autonomous running timer, the system timer interrupt is used as timebase
 *  - timer can work in one shot mode or in auto reload mode
 *  - when a timer expires, it triggers a semaphore event
 *
 * <b>Software Interrupts:</b>
 *  - up to 256 simulated software interrupts on 8 bit processors
 *  - can be used to interface fast hardware interrupts outside the scope of
 *    pico]OS to the operating system
 *
 * <b>Miscellaneous:</b>
 *  - atomic variables
 *  - blocking and nonblocking lists
 *  - debugging support allows access to internal structures
 *
 * @n
 * The optional <b>nano-layer</b> supports:
 *
 *  - Bottom Halfs for interrupt service routines
 *  - Multitasking able console I/O
 *  - Multitasking able dynamic memory management
 *  - Standardized API for easier task creation
 *  - Named tasks, semaphores, mutexes and timer
 *  - CPU usage measurement
 *
 * @n@n
 * @subsection ports Available Ports
 *
 * Currently, ports to the following platforms are available:
 *
 *  - Intel 80x86 in real mode, the executable is loadable from DOS
 *  - MS Windows 32bit port, usefull for software development and debugging
 *  - 6502 / 65c02 / 6510 series, port for Commodore 64 available.
 *    Minimum configuration (4 tasks): 4.6kb code, 4.7kb data
 *  - PowerPC: IBM PPC440  (GNU C and MetaWare compiler supported)
 *  - AVR (ATMega, GNU C compiler supported)
 *  - ARM: SAMSUNG S3C2510A CPU (ARM940T core), Philips LPC ARM chips,
 *         Cortex-M0 and Cortex-M3
 *  - Texas Instruments MSP430 family
 *  - Unix (using setcontext/getcontext), for development and testing purpose only)
 *  - MyCPU (see http://www.mycpu.eu)
 *
 * @n@n
 * @subsection files Files
 *
 * The pico]OS Real Time Operating System consists of only
 * a couple of files:
 *
 *  - picoos.h 
 *       - This header is the interface to the operating system.
 *         It is included by the pico]OS core and must also be
 *         included by user applications. 
 *  - poscfg.h
 *       - pico]OS configuration file. Must be supplied by the user.
 *  - noscfg.h
 *       - nano layer configuration file. Must be supplied by the user.
 *  - port.h
 *       - Configuration file for the port.
 *         Must be supplied by the user / port developer.
 *  - picoos.c
 *       - The pico]OS core source file. This is the operating system.
 *  - n_xxxx.c
 *       - The nano layer source files.
 *  - fbit_gen.c 
 *       - This file implements a set of generic "findbit" functions.
 *         The "findbit" function is needed by the operating system.
 *         The user may support its own "findbit" function to
 *         increase performance on a particular architecture.
 *  - arch_c.c
 *       - User supplied file. This is the architecture specific C-code
 *         of the platform port.
 *  - arch_a.asm
 *       - User supplied file (optional). This is the architecture specific
 *         assembler code of the platform port.
 * 
 * @n@n
 * @section license License
 *
 *  Copyright (c) 2004-2012, Dennis Kuschel. @n
 *  All rights reserved. @n
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met: @n
 *
 *   -# Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   -# Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   -# The name of the author may not be used to endorse or promote
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
 *  OF THE POSSIBILITY OF SUCH DAMAGE. @n
 *
 * @n@n
 * @section cont Contact Information
 * Dennis Kuschel @n
 * Kurt-Georg-Kiesinger-Allee 14 @n
 * 28279 Bremen @n
 * GERMANY @n
 *
 * mail: dennis_k@freenet.de @n
 * web:  http://picoos.sourceforge.net @n
 *
 * (C) 2004-2012 Dennis Kuschel
 */

/** @defgroup intro     Introduction */
/** @defgroup configp   Configuration: Pico Layer */
/** @defgroup confign   Configuration: Nano Layer */
/** @defgroup userapip  User API: Pico Layer */
/** @defgroup userapin  User API: Nano Layer */

/** @defgroup layer The Layer Scheme
 * @ingroup intro
 * @{
 *
 * pico]OS is currently split into two layers:
 *
 *   - The pico layer contains the RTOS core. It covers:
 *      - Tasks
 *      - Semaphores
 *      - Mutexes
 *      - Flag Events
 *      - Message Boxes
 *      - Software Interrupts
 *      - Timers
 *      - Lists
 *      - Atomic Variables
 *
 *   - The nano layer contains features that are based on the pico layer:
 *      - Bottom Halfs
 *      - Multitasking able console I/O
 *      - Multitasking able dynamic memory management
 *      - Standardized API for easier task creation
 *      - Named tasks, semaphores, mutexes and timer
 *      - CPU usage measurement
 *      - etc. (more will follow)
 *
 * Below is a diagram showing the layer scheme. Note that the nano layer
 * is disabled by default to lower the memory usage for small devices.
 * There are two ways to enable the nano layer: First, if you run your
 * makefile, you can add the parameter @c NANO=1 to the make command line.
 * Second, you can set this parameter in your application makefile. @n
 * If the parameter @c NANO is set to 1, the makesystem will automatically
 * compile and link the nano layer source files. Also the global define
 * @c POSCFG_ENABLE_NANO is set, so you can test from within your sources
 * if the nano layer is enabled or not. @n@n
 *
 * <p><img src="../pic/layer.png" align="middle" border=0></p>@n@n@n
 *
 * @}
 */

#ifndef _PICOOS_H
#define _PICOOS_H


#define POS_VER_N           0x0104
#define POS_VER_S           "1.0.4"
#define POS_COPYRIGHT       "(c) 2004-2012, D.Kuschel"
#define POS_STARTUPSTRING   "pico]OS " POS_VER_S "  " POS_COPYRIGHT

#ifndef NULL
#ifndef _HAVE_NULL
#define _HAVE_NULL
#define NULL ((void*)0)
#endif
#endif

#ifndef DOX
#define DOX  0
#endif
#if DOX==0

#ifndef _POSPACK
#ifdef __MYCPU__
#define PICOSUBDIR  1
#endif
#endif

#ifdef PICOSUBDIR
/* required because of stupid CC65 compiler */
#include <picoos/port.h>
#include <picoos/poscfg.h>
#else
#include <port.h>
#include <poscfg.h>
#endif


/*---------------------------------------------------------------------------
 *  CONFIGURATION
 *-------------------------------------------------------------------------*/

/* test if all needed parameters are set */
#ifndef MVAR_t
#error  MVAR_t not defined
#endif
#ifndef MVAR_BITS
#error  MVAR_BITS not defined
#endif
#ifndef POSCFG_ALIGNMENT
#error  POSCFG_ALIGNMENT not defined
#endif
#ifndef POSCFG_MAX_PRIO_LEVEL
#error  POSCFG_MAX_PRIO_LEVEL not defined
#endif
#ifndef POSCFG_MAX_TASKS
#error  POSCFG_MAX_TASKS not defined
#endif
#ifndef POSCFG_MAX_EVENTS
#error  POSCFG_MAX_EVENTS not defined
#endif
#ifndef POSCFG_MAX_MESSAGES
#error  POSCFG_MAX_MESSAGES not defined
#endif
#ifndef POSCFG_MAX_TIMER
#error  POSCFG_MAX_TIMER not defined
#endif
#ifndef POSCFG_SOFT_MTASK
#error  POSCFG_SOFT_MTASK not defined
#endif
#ifndef POSCFG_CTXSW_COMBINE
#error  POSCFG_CTXSW_COMBINE not defined
#endif
#ifndef POSCFG_REALTIME_PRIO
#error  POSCFG_REALTIME_PRIO not defined
#endif
#ifndef POSCFG_ISR_INTERRUPTABLE
#error  POSCFG_ISR_INTERRUPTABLE not defined
#endif
#ifndef POSCFG_ROUNDROBIN
#error  POSCFG_ROUNDROBIN not defined
#endif
#ifndef POSCFG_TASKS_PER_PRIO
#error  POSCFG_TASKS_PER_PRIO not defined
#endif
#ifndef POSCFG_FASTCODE
#error  POSCFG_FASTCODE not defined
#endif
#ifndef POSCFG_SMALLCODE
#error  POSCFG_SMALLCODE not defined
#endif
#ifndef POSCFG_ARGCHECK
#error  POSCFG_ARGCHECK not defined
#endif
#ifndef POSCFG_MSG_MEMORY
#error  POSCFG_MSG_MEMORY not defined
#endif
#ifndef POSCFG_MSG_BUFSIZE
#error  POSCFG_MSG_BUFSIZE not defined
#endif
#ifndef POSCFG_DYNAMIC_MEMORY
#error  POSCFG_DYNAMIC_MEMORY not defined
#endif
#ifndef POSCFG_DYNAMIC_REFILL
#error  POSCFG_DYNAMIC_REFILL
#endif
#if POSCFG_DYNAMIC_MEMORY != 0
#ifndef POS_MEM_ALLOC
#error  POS_MEM_ALLOC not defined
#endif
#ifndef POS_MEM_ALLOC
#error  POS_MEM_ALLOC not defined
#endif
#endif
#ifndef POSCFG_CALLINITARCH
#error  POSCFG_CALLINITARCH not defined
#endif
#ifndef POSCFG_LOCK_USEFLAGS
#error  POSCFG_LOCK_USEFLAGS not defined
#endif
#if POSCFG_LOCK_USEFLAGS != 0
#ifndef POSCFG_LOCK_FLAGSTYPE
#error  POSCFG_LOCK_FLAGSTYPE not defined
#endif
#endif
#ifndef POS_SCHED_LOCK
#error  POS_SCHED_LOCK not defined
#endif
#ifndef POS_SCHED_UNLOCK
#error  POS_SCHED_UNLOCK not defined
#endif
#ifndef HZ
#error  HZ not defined
#endif
#ifndef POSCFG_TASKSTACKTYPE
#error  POSCFG_TASKSTACKTYPE not defined
#endif
#ifndef POS_USERTASKDATA
#error  POS_USERTASKDATA not defined
#endif
#ifndef POSCFG_FEATURE_YIELD
#error  POSCFG_FEATURE_YIELD not defined
#endif
#ifndef POSCFG_FEATURE_SLEEP
#error  POSCFG_FEATURE_SLEEP not defined
#endif
#ifndef POSCFG_FEATURE_EXIT
#error  POSCFG_FEATURE_EXIT not defined
#endif
#ifndef POSCFG_FEATURE_GETPRIORITY 
#error  POSCFG_FEATURE_GETPRIORITY  not defined
#endif
#ifndef POSCFG_FEATURE_SETPRIORITY
#error  POSCFG_FEATURE_SETPRIORITY not defined
#endif
#ifndef POSCFG_FEATURE_SEMAPHORES
#error  POSCFG_FEATURE_SEMAPHORES not defined
#endif
#ifndef POSCFG_FEATURE_SEMADESTROY
#error  POSCFG_FEATURE_SEMADESTROY not defined
#endif
#ifndef POSCFG_FEATURE_SEMAWAIT
#error  POSCFG_FEATURE_SEMAWAIT not defined
#endif
#ifndef POSCFG_FEATURE_MUTEXES
#error  POSCFG_FEATURE_MUTEXES not defined
#endif
#ifndef POSCFG_FEATURE_MUTEXDESTROY
#error  POSCFG_FEATURE_MUTEXDESTROY not defined
#endif
#ifndef POSCFG_FEATURE_MUTEXTRYLOCK
#error  POSCFG_FEATURE_MUTEXTRYLOCK not defined
#endif
#ifndef POSCFG_FEATURE_GETTASK
#error  POSCFG_FEATURE_GETTASK not defined
#endif
#ifndef POSCFG_FEATURE_TASKUNUSED
#error  POSCFG_FEATURE_TASKUNUSED not defined
#endif
#ifndef POSCFG_FEATURE_MSGBOXES
#error  POSCFG_FEATURE_MSGBOXES not defined
#endif
#ifndef POSCFG_FEATURE_MSGWAIT
#error  POSCFG_FEATURE_MSGWAIT not defined
#endif
#ifndef POSCFG_FEATURE_INHIBITSCHED
#error  POSCFG_FEATURE_INHIBITSCHED not defined
#endif
#ifndef POSCFG_FEATURE_JIFFIES
#error  POSCFG_FEATURE_JIFFIES not defined
#endif
#ifndef POSCFG_FEATURE_TIMER
#error  POSCFG_FEATURE_TIMER not defined
#endif
#ifndef POSCFG_FEATURE_TIMERDESTROY
#error  POSCFG_FEATURE_TIMERDESTROY not defined
#endif
#ifndef POSCFG_FEATURE_TIMERFIRED
#error  POSCFG_FEATURE_TIMERFIRED not defined
#endif
#ifndef POSCFG_FEATURE_FLAGS
#error  POSCFG_FEATURE_FLAGS not defined
#endif
#ifndef POSCFG_FEATURE_FLAGDESTROY
#error  POSCFG_FEATURE_FLAGDESTROY not defined
#endif
#ifndef POSCFG_FEATURE_FLAGWAIT
#error  POSCFG_FEATURE_FLAGWAIT not defined
#endif
#ifndef POSCFG_FEATURE_SOFTINTS
#error  POSCFG_FEATURE_SOFTINTS not defined
#else
#ifndef POSCFG_FEATURE_SOFTINTDEL
#error  POSCFG_FEATURE_SOFTINTDEL not defined
#endif
#ifndef POSCFG_SOFTINTERRUPTS
#error  POSCFG_SOFTINTERRUPTS not defined
#endif
#ifndef POSCFG_SOFTINTQUEUELEN
#error  POSCFG_SOFTINTQUEUELEN not defined
#endif
#endif
#ifndef POSCFG_FEATURE_IDLETASKHOOK
#error  POSCFG_FEATURE_IDLETASKHOOK not defined
#endif
#ifndef POSCFG_FEATURE_ERRNO
#error  POSCFG_FEATURE_ERRNO not defined
#endif
#ifndef POSCFG_FEATURE_ATOMICVAR
#error  POSCFG_FEATURE_ATOMICVAR not defined
#endif
#ifndef POSCFG_FEATURE_LISTS
#error  POSCFG_FEATURE_LISTS not defined
#else
#ifndef POSCFG_FEATURE_LISTJOIN
#error  POSCFG_FEATURE_LISTJOIN not defined
#endif
#ifndef POSCFG_FEATURE_LISTLEN
#define POSCFG_FEATURE_LISTLEN  1
#endif
#endif
#ifndef POSCFG_FEATURE_DEBUGHELP
#define POSCFG_FEATURE_DEBUGHELP  0
#endif
#ifdef _DBG
#if POSCFG_FEATURE_DEBUGHELP
#define POS_DEBUGHELP
#endif
#endif
#ifndef POSCFG_PORTMUTEX
#define POSCFG_PORTMUTEX  0
#endif
#ifndef POSCFG_INT_EXIT_QUICK
#define POSCFG_INT_EXIT_QUICK 0
#endif

/* parameter range checking */
#if (POSCFG_DYNAMIC_MEMORY != 0) && (POSCFG_DYNAMIC_REFILL != 0)
#define SYS_POSTALLOCATE    1
#else
#define SYS_POSTALLOCATE    0
#endif
#if (MVAR_BITS != 8) && (MVAR_BITS != 16) && (MVAR_BITS != 32)
#error MVAR_BITS must be 8, 16 or 32
#endif
#if POSCFG_MAX_PRIO_LEVEL == 0
#error POSCFG_MAX_PRIO_LEVEL must not be zero
#endif
#if (POSCFG_ROUNDROBIN != 0) && (POSCFG_MAX_PRIO_LEVEL > MVAR_BITS)
#error POSCFG_MAX_PRIO_LEVEL must not exceed MVAR_BITS
#endif 
#if (POSCFG_ROUNDROBIN == 0) && (POSCFG_MAX_PRIO_LEVEL > (MVAR_BITS*MVAR_BITS))
#error POSCFG_MAX_PRIO_LEVEL must not exceed (MVAR_BITS * MVAR_BITS)
#endif 
#if (POSCFG_MAX_TASKS < 2) && (SYS_POSTALLOCATE == 0)
#error POSCFG_MAX_TASKS is less than 2
#endif
#if (POSCFG_MAX_TASKS > (POSCFG_TASKS_PER_PRIO * POSCFG_MAX_PRIO_LEVEL))
#error POSCFG_MAX_TASKS is much to big
#endif
#if (POSCFG_MAX_EVENTS < 1) && (SYS_POSTALLOCATE == 0)
#error POSCFG_MAX_EVENTS must be at least 1
#endif
#if POSCFG_TASKS_PER_PRIO > MVAR_BITS
#error POSCFG_TASKS_PER_PRIO can not exceed MVAR_BITS
#endif
#if (POSCFG_TASKS_PER_PRIO < 2) && (POSCFG_ROUNDROBIN != 0)
#error POSCFG_TASKS_PER_PRIO must be at least 2
#endif
#if (POSCFG_TASKS_PER_PRIO != 1) && (POSCFG_ROUNDROBIN == 0)
#error POSCFG_TASKS_PER_PRIO must be 1 when round robin scheduling is disabled
#endif
#if ((POSCFG_TASKS_PER_PRIO % 3) == 0) || ((POSCFG_TASKS_PER_PRIO % 5) == 0)
#error POSCFG_TASKS_PER_PRIO must be 1/2/4/8/16 or 32
#endif
#if (POSCFG_REALTIME_PRIO >= POSCFG_MAX_PRIO_LEVEL) && (POSCFG_ROUNDROBIN != 0)
#error POSCFG_REALTIME_PRIO must be less than POSCFG_MAX_PRIO_LEVEL
#endif
#if POSCFG_FEATURE_MSGBOXES != 0
#if (POSCFG_MAX_MESSAGES < 2) && (SYS_POSTALLOCATE == 0)
#error POSCFG_MAX_MESSAGES must be at least 2
#endif
#if POSCFG_MSG_BUFSIZE < 1
#error POSCFG_MSG_BUFSIZE must be at least 1
#endif
#endif
#if (POSCFG_FEATURE_TIMER != 0)
#if (POSCFG_MAX_TIMER == 0) && (SYS_POSTALLOCATE == 0)
#error POSCFG_MAX_TIMER must be at least 1
#endif
#endif
#if (POSCFG_TASKSTACKTYPE < 0) || (POSCFG_TASKSTACKTYPE > 2)
#error POSCFG_TASKSTACKTYPE must be 0, 1 or 2
#endif
#if POSCFG_ALIGNMENT == 0
#undef POSCFG_ALIGNMENT
#define POSCFG_ALIGNMENT  1
#else
#if ((POSCFG_ALIGNMENT % 3) == 0) || ((POSCFG_ALIGNMENT % 5) == 0)
#error POSCFG_ALIGNMENT must be 1/2/4/8/16/32/64/128/256 etc.
#endif
#endif
#if POSCFG_FEATURE_SOFTINTS != 0
#if POSCFG_SOFTINTERRUPTS == 0
#error POSCFG_SOFTINTERRUPTS must be greater than 0
#endif
#if POSCFG_SOFTINTQUEUELEN < 2
#error POSCFG_SOFTINTQUEUELEN must be at least 2
#endif
#endif


/* parameter reconfiguration */
#ifndef POSCFG_TASKCB_USERSPACE
#define POSCFG_TASKCB_USERSPACE     0
#endif
#if POSCFG_FEATURE_SEMAPHORES == 0
#undef POSCFG_FEATURE_SEMADESTROY
#define POSCFG_FEATURE_SEMADESTROY  0
#endif
#if POSCFG_FEATURE_FLAGS == 0
#undef POSCFG_FEATURE_FLAGDESTROY
#define POSCFG_FEATURE_FLAGDESTROY  0
#endif
#if POSCFG_FEATURE_MUTEXES == 0
#undef POSCFG_FEATURE_MUTEXDESTROY
#define POSCFG_FEATURE_MUTEXDESTROY  0
#else
#if (POSCFG_FEATURE_MUTEXDESTROY != 0) && (POSCFG_FEATURE_SEMADESTROY == 0)
#undef POSCFG_FEATURE_SEMADESTROY
#define POSCFG_FEATURE_SEMADESTROY 1
#endif
#endif
#if (POSCFG_FEATURE_MSGBOXES != 0) && (SYS_POSTALLOCATE == 0)
#define SYS_MSGBOXEVENTS  2
#else
#define SYS_MSGBOXEVENTS  0
#endif
#if (POSCFG_FEATURE_MSGBOXES != 0) && (POSCFG_FEATURE_GETTASK == 0)
#undef POSCFG_FEATURE_GETTASK
#define POSCFG_FEATURE_GETTASK 1
#endif
#if (POSCFG_FEATURE_SEMAWAIT != 0) || (POSCFG_FEATURE_MSGWAIT != 0)
#define SYS_TASKDOUBLELINK  1
#else
#define SYS_TASKDOUBLELINK  0
#endif
#define SYS_EVENTS_USED  \
      (POSCFG_FEATURE_MUTEXES | POSCFG_FEATURE_MSGBOXES | \
       POSCFG_FEATURE_FLAGS | POSCFG_FEATURE_LISTS)
#define SYS_FEATURE_EVENTS  (POSCFG_FEATURE_SEMAPHORES | SYS_EVENTS_USED)
#define SYS_FEATURE_EVENTFREE  (POSCFG_FEATURE_SEMADESTROY | \
          POSCFG_FEATURE_MUTEXDESTROY | POSCFG_FEATURE_FLAGDESTROY | \
          POSCFG_FEATURE_LISTS)
#if (POSCFG_FEATURE_MSGBOXES != 0) && (POSCFG_FEATURE_EXIT != 0)
#undef  SYS_FEATURE_EVENTFREE
#define SYS_FEATURE_EVENTFREE  1
#endif
#ifndef POSCFG_FEATURE_LARGEJIFFIES
#if MVAR_BITS < 16
#define POSCFG_FEATURE_LARGEJIFFIES  1
#else
#define POSCFG_FEATURE_LARGEJIFFIES  0
#endif
#endif
#ifndef POSCALL
#define POSCALL
#endif
#if POSCFG_FEATURE_SETPRIORITY != 0
#define SYS_TASKEVENTLINK  1
#else
#define SYS_TASKEVENTLINK  0
#endif

#endif /* DOX!=0 */



/*---------------------------------------------------------------------------
 *  SOME DEFINITIONS AND MACROS
 *-------------------------------------------------------------------------*/

#ifdef _POSCORE_C
#define POSEXTERN
#define POSFROMEXT  extern
#else
#define POSEXTERN   extern
#define POSFROMEXT
#endif

#if POSCFG_ROUNDROBIN == 0
#define SYS_TASKTABSIZE_X  MVAR_BITS
#define SYS_TASKTABSIZE_Y  ((POSCFG_MAX_PRIO_LEVEL+MVAR_BITS-1)/MVAR_BITS)
#else
#define SYS_TASKTABSIZE_X  POSCFG_TASKS_PER_PRIO
#define SYS_TASKTABSIZE_Y  POSCFG_MAX_PRIO_LEVEL
#endif

#define SYS_TASKSTATE (POSCFG_FEATURE_TASKUNUSED | POSCFG_FEATURE_MSGBOXES)

#if POSCFG_LOCK_USEFLAGS != 0
#define POS_LOCKFLAGS   POSCFG_LOCK_FLAGSTYPE flags
#else
#define POS_LOCKFLAGS   do { } while(0)
#endif

#define POSTASKSTATE_UNUSED      0
#define POSTASKSTATE_ZOMBIE      1
#define POSTASKSTATE_ACTIVE      2

#define INFINITE   ((UINT_t)~0)



/*---------------------------------------------------------------------------
 *  ERROR CODES
 *-------------------------------------------------------------------------*/

/** @defgroup errcodes Error Codes
 * @ingroup userapip
 * Most pico]OS functions return a signed integer value describing the
 * execution status of the function. Generally, a negative value denotes
 * an error, zero means success (no error), and a positive value may
 * be returned as result of a successful operation.
 * To check if an operation was successful, you need only to test if the
 * returned value is positiv ( status >= 0 ).  For example, if you would
 * like to test if a function failed because the system ran out of memory,
 * you may test the status against the (negativ) value @c -E_NOMEM.
 * @{
 */

/** @brief  No Error (the operation was successful) */
#define E_OK        0

/** @brief  The operation failed. */
#define E_FAIL      1

/** @brief  The System ran out of memory. */
#define E_NOMEM     2

/** @brief  The given argument is wrong or inacceptable. */
#define E_ARG       3

/** @brief  The operation is forbidden at the current operation stage. */
#define E_FORB      4

/** @brief  For query operations: End of query (no more elements) */
#define E_NOMORE    5

/** @brief  The query operation was not successfull */
#define E_NOTFOUND  6

/** @} */



/*---------------------------------------------------------------------------
 *  MACROS
 *-------------------------------------------------------------------------*/

/** Convert milliseconds into HZ timer ticks.
 * This macro is used to convert milliseconds into the timer tick
 * rate (see ::HZ define).
 * Example:  Use  ::posTaskSleep(MS(1000))  to sleep 1000 ms.
 */
#if (DOX==0) && (HZ <= 1000)
#define MS(msec)  (((msec)<(1000/HZ)) ? \
                    ((UINT_t)1) : ((UINT_t)((1L*HZ*(msec))/1000)))
#else
#define MS(msec)  ((UINT_t)((1L*HZ*(msec))/1000))
#endif



/*---------------------------------------------------------------------------
 *  DATA TYPES
 *-------------------------------------------------------------------------*/

/** @brief  Signed machine variable type.
 * This variable type is the fastest for
 * the target architecture.
 * @sa UVAR_t
 */
typedef signed MVAR_t     VAR_t;

/** @brief  Unsigned machine variable type.
 * This variable type is the fastest for
 * the target architecture.
 * @sa VAR_t
 */
typedef unsigned MVAR_t   UVAR_t;

#ifndef MINT_t
#define MINT_t int
#endif
/** @brief  Signed integer.
 *
 * The bit size can be changed by the user
 * by defining MINT_t to something other than @e int
 * in the port configuration file.
 * This integer type is used by the operating system e.g.
 * for semaphore counters and timer.
 * @sa UINT_t
 */
typedef signed MINT_t     INT_t;

/** @brief  Unsigned integer.
 *
 * The bit size can be changed by the user
 * by defining MINT_t to something other than @e int
 * in the pico]OS configuration file.
 * This integer type is used by the operating system e.g.
 * for semaphore counters and timer.
 * @sa INT_t
 */
typedef unsigned MINT_t   UINT_t;

#ifndef MPTR_t
#define MPTR_t long
#endif
/** @brief Memory pointer type.
 *
 * This variable type is an integer with the width
 * of the address lines of memory architecure. The bit width
 * is equal to the width of a void-pointer.
 * This variable type is needed by the operating system for
 * lossless typecasting of void pointers to integers.
 * ::MPTR_t is a define that is set in the port configuration
 * file. When ::MPTR_t is not set, it defaults to @e long.
 * @sa MPTR_t
 */
typedef unsigned MPTR_t   MEMPTR_t;

#if (DOX!=0) || (POSCFG_FEATURE_LARGEJIFFIES == 0)
/** @brief  Signed type of ::JIF_t.
 * @sa JIF_t
 */
typedef VAR_t             SJIF_t;
/** @brief  Timer counter type. Can be ::UVAR_t or ::UINT_t.
 * @sa SJIF_t
 */
typedef UVAR_t            JIF_t;
#else
typedef INT_t             SJIF_t;
typedef UINT_t            JIF_t;
#endif

/** @brief  Generic function pointer.
 * @param arg  optional argument, can be NULL if not used.
 */
typedef void (*POSTASKFUNC_t)(void* arg);

/** @brief  Software interrupt callback function pointer.
 *
 * The parameter @e arg is the value that was dropped
 * in the call to ::posSoftInt.
 * @sa posSoftInt
 */
typedef void (*POSINTFUNC_t)(UVAR_t arg);

#if (DOX!=0) ||(POSCFG_FEATURE_IDLETASKHOOK != 0)
/** @brief  Idle task function pointer */
typedef void (*POSIDLEFUNC_t)(void);
#endif

/* forward declarations (just dummies) */
struct POSSEMA;
struct POSMUTEX;
struct POSFLAG;
struct POSTIMER;

/** @brief  Handle to a semaphore object.
 * @sa posSemaCreate, posSemaGet, posSemaWait, posSemaSignal
 */
typedef struct POSSEMA *POSSEMA_t;

/** @brief  Handle to a mutex object.
 * @sa posMutexCreate, posMutexLock, posMutexTryLock, posMutexUnlock
 */
typedef struct POSMUTEX *POSMUTEX_t;

/** @brief  Handle to a flag object.
 * @sa posFlagCreate, posFlagDestroy, posFlagGet, posFlagSet
 */
typedef struct POSFLAG *POSFLAG_t;

/** @brief  Handle to a timer object.
 * @sa posTimerCreate, posTimerDestroy, posTimerSet, posTimerStart
 */
typedef struct POSTIMER *POSTIMER_t;

/** @brief  Atomic variable.
 * @sa posAtomicGet, posAtomicSet, posAtomicAdd, posAtomicSub
 */
typedef volatile INT_t  POSATOMIC_t;

#if (DOX!=0) || (POSCFG_FEATURE_LISTS != 0)
struct POSLIST;
struct POSLISTHEAD {
  struct POSLIST* volatile  prev;
  struct POSLIST* volatile  next;
  POSSEMA_t       volatile  sema;
  UVAR_t          volatile  flag;
#if POSCFG_FEATURE_LISTLEN != 0
  UINT_t          volatile  length;
#endif
};
struct POSLIST {
  struct POSLIST* volatile  prev;
  struct POSLIST* volatile  next;
#if POSCFG_FEATURE_LISTLEN != 0
  struct POSLISTHEAD* volatile head;
#endif
};
/** @brief  List variable.
 * This variable type is used as running variable of a list or as list link.
 * @sa POSLISTHEAD_t, posListInit, posListTerm, posListAdd, posListGet
 */
typedef struct POSLIST POSLIST_t;
/** @brief  List variable.
 * This variable defines the head of a list.
 * @sa POSLIST_t, posListInit, posListTerm, posListAdd, posListGet
 */
typedef struct POSLISTHEAD POSLISTHEAD_t;
#endif


/** @brief  Task environment structure.
 *
 * Most members of this structure are private, and are hidden from the user.
 * The user can add its own members to the structure. For this purpose the
 * user must define the macro ::POS_USERTASKDATA in the pico]OS
 * configuration file. Here is an example of this macro:@n
 *
 * @code
 * #define POS_USERTASKDATA \
 *   void            *stackptr; \
 *   unsigned short  stack[FIXED_STACK_SIZE]; \
 *   int             errno;
 * @endcode
 *
 * Note that the stackptr variable is required by most of the architecture
 * ports. The stack array is an example of how to include the stack frame
 * into the task environment structure (e.g. when ::POSCFG_TASKSTACKTYPE
 * is defined to 2).
 */
typedef struct POSTASK  *POSTASK_t; /* forward declaration */



/*---------------------------------------------------------------------------
 *  DEFINITIONS FOR GENERIC "findbit" FUNCTION  (file fbit_gen.c)
 *-------------------------------------------------------------------------*/

#ifndef POSCFG_FBIT_USE_LUTABLE
#define POSCFG_FBIT_USE_LUTABLE  0
#endif
#if (POSCFG_FBIT_USE_LUTABLE > 1) && (POSCFG_ROUNDROBIN == 0)
#undef POSCFG_FBIT_USE_LUTABLE
#define POSCFG_FBIT_USE_LUTABLE 1
#endif
#ifndef FINDBIT
#if POSCFG_FBIT_USE_LUTABLE == 1
#if POSCFG_ROUNDROBIN == 0
#ifndef _FBIT_GEN_C
extern VAR_t const p_pos_fbittbl[256];
#endif
#define FINDBIT(x)  p_pos_fbittbl[x] 
#else
UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset);
#define FINDBIT(x, o)  p_pos_findbit(x, o)
#endif
#elif POSCFG_FBIT_USE_LUTABLE == 2
#ifndef _FBIT_GEN_C
extern VAR_t const p_pos_fbittbl_rr[8][256];
#endif
#define FINDBIT(x, o)  p_pos_fbittbl_rr[o][x]
#endif
#endif /* !FINDBIT */



/*---------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *-------------------------------------------------------------------------*/

/** @brief  Global task variable.
 * This variable points to the environment structure of the currently
 * active task.
 * @note  Only the context switch functions ::p_pos_softContextSwitch,
 *        ::p_pos_intContextSwitch and ::p_pos_startFirstContext
 *        need to access this variable.
 * @sa posNextTask_g
 */
POSEXTERN volatile POSTASK_t posCurrentTask_g;

/** @brief  Global task variable.
 * This variable points to the environment structure of the next task
 * that shall be scheduled.
 * @note  The context switch functions ::p_pos_softContextSwitch and
 *        ::p_pos_intContextSwitch must copy the content of this variable
 *        into the ::posCurrentTask_g variable.
 * @sa posCurrentTask_g
 */
POSEXTERN volatile POSTASK_t posNextTask_g;

/** @brief  Global flag variable.
 * This variable is nonzero when the CPU is currently executing an
 * interrupt service routine.
 * @note  Only the architecture specific ISR functions need to access
 *        this variable.
 */
#ifndef _POSCORE_C
POSEXTERN volatile UVAR_t    posInInterrupt_g;
#else
POSEXTERN volatile UVAR_t    posInInterrupt_g = 1;
#endif

/** @brief  Global flag variable.
 * This variable is nonzero when the operating system is initialized
 * and running.
 * @note  Only the architecture specific ISR functions need to access
 *        this variable.
 */
#ifndef _POSCORE_C
POSEXTERN volatile UVAR_t    posRunning_g;
#else
POSEXTERN volatile UVAR_t    posRunning_g = 0;
#endif


#if DOX!=0
/** @brief  Unix style error variable.
 * This variable is global for the currently runnig task.
 * @note ::POSCFG_FEATURE_ERRNO must be set to 1 to enable this variable.
 * @sa POSCFG_FEATURE_ERRNO, E_OK
 */
VAR_t   errno;
#endif
#if POSCFG_FEATURE_ERRNO != 0
#ifdef errno
#undef errno
#endif
POSEXTERN VAR_t* _errno_p(void);
#define errno (*_errno_p())
#endif



/*---------------------------------------------------------------------------
 *  PROTOTYPES OF INTERNAL FUNCTIONS
 *-------------------------------------------------------------------------*/

/** @defgroup port pico]OS Porting Information
 * @ingroup intro
 * <h3>General Information</h3>
 * <h4>Choose the best type of stack management</h4>
 * The operating system can be easily ported to other architectures,
 * it can be ported to very small 8 bit architectures with low memory
 * and to 32 bit architectures with lots of memory. To keep the
 * porting as simple as possible, there are only a couple of functions
 * that must be adapted to the architecute.
 * Before you start porting the operating system to your architecture,
 * you must choose a stack management type. You have the choice
 * between:@n
 * 
 * ::POSCFG_TASKSTACKTYPE <b>= 0</b>@n
 * The stack memory is provided by the user. This is the best choice for
 * very small architectures with low memory.@n
 *
 * ::POSCFG_TASKSTACKTYPE <b>= 1</b>@n
 * The stack memory is dynamically allocated by the architecture dependent
 * code of the operating system. The size of the stack frame is variable
 * and can be choosen by the user who creates the task. This is the best
 * choice for big architectures with lots of memory.@n
 * 
 * ::POSCFG_TASKSTACKTYPE <b>= 2</b>@n
 * The stack memory is dynamically allocated by the architecture dependent
 * code of the operating system. The size of the stack frame is fixed and
 * can not be changed by the user. This may be an alternative to type 0,
 * it is a little bit more user friendly.@n
 * 
 * Here is a list of the functions that are architecture specific and 
 * must be ported:@n
 * ::p_pos_initTask, ::p_pos_startFirstContext, ::p_pos_softContextSwitch,
 * ::p_pos_intContextSwitch.@n
 *
 * If you choose ::POSCFG_TASKSTACKTYPE <b>= 2</b> or <b>3</b>, you must
 * also provide the function ::p_pos_freeStack.@n@n@n
 *
 * <h4>Get more speed with optimized "findbit" function</h4>
 * If your application is critical in performance, you may also provide
 * an assembler version of the function "findbit".
 * There are two different function prototypes possible. The simple
 * prototype for the standard scheduling scheme
 * (::POSCFG_ROUNDROBIN <b>= 0</b>) is@n
 *
 * <b>UVAR_t ::p_pos_findbit(const UVAR_t bitfield);</b>
 *
 * The prototype for a findbit function that supports round robin
 * scheduling (::POSCFG_ROUNDROBIN <b>= 1</b>) is@n
 *
 * <b>UVAR_t ::p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset);</b>@n
 *
 * The function gets a bitfield as input, and returns the number of the
 * right most set bit (that is the number of the first lsb that is set).
 * If round robin is enabled, the function takes an offset as second
 * parameter. The offset is the position where the function starts to
 * search the first set bit. The function scans the bitfield always from
 * right to left, starting with the bit denoted by the offset. The bitfield
 * is seen as circle, when the rightmost bit is not set the function
 * must continue scanning the leftmost bit (wrap around), so all bits
 * of the field are scanned.@n
 * It is possible to implement the findbit mechanism as look up table.
 * For this purpose you can define the macro @b FINDBIT. Please see the
 * header file picoos.h (search for the word ::POSCFG_FBIT_USE_LUTABLE)
 * and the source file fbit_gen.c for details.@n
 *
 * @n<h3>Assembler Functions</h3>
 * Unfortunately, not the whole operating system can be written in C.
 * The platform port must be written in assembly language. I tried to
 * keep the assembly part of the RTOS as small as possible. But there
 * are three assembly functions left, that are needed for doing
 * the context switching:@n
 *
 *   - ::p_pos_startFirstContext
 *   - ::p_pos_softContextSwitch
 *   - ::p_pos_intContextSwitch
 *
 * The operating system requires also a timer interrupt that is used
 * to cut the task execution time into slices. Hardware interrupts
 * must comply with some conventions to be compatible to pico]OS.
 * So the fourth thing you need to write in assember is a framework
 * for hardware interrupts.@n
 *
 * The diagram shows the assembler functions in logical structure.
 * At the left side I have drawn a normal interrupt service routine
 * for reference.@n@n@n
 *
 * <p><img src="../pic/portfc1.png" align="middle" border=0></p>@n
 *
 * The context switching (multitasking) is done by simply swaping the
 * stack frame when an interrupt service routine (eg. the timer interrupt)
 * is left. But it must also be possible for a task to give of processing
 * time without the need of an interrupt.
 * This is done by the function ::p_pos_softContextSwitch
 * at the right side in the diagram. Since this function is not called
 * by a processor interrupt, it must build up an ISR compatible
 * stack frame by itself. Note that the second part of this function is
 * equal to the function ::p_pos_intContextSwitch, so the function must be
 * terminated by an return-from-interrupt instruction, even if the
 * function was called from a C-routine.@n
 *
 * For completeness, the next diagram shows at its left side how
 * the function ::p_pos_startFirstContext works. Again, this function
 * looks like the lower part of the funtion ::p_pos_intContextSwitch
 * in the diagram above. In the middle you can see how the timer
 * interrupt routine must look like.@n@n@n
 *
 * <p><img src="../pic/portfc2.png" align="middle" border=0></p>@n
 *
 * There is a special interrupt handling needed when  interrupts are
 * interruptable on your system. To prevent a deadlock situation (that
 * is, an ISR would be called again and again until the stack flows over),
 * a counting flag variable is exported by pico]OS: ::posInInterrupt_g.
 * This variable contains the value zero if no interrupt is running yet.
 * And only if no other interrupt is running, the ISR must save the
 * stack pointer to the task environment structure where ::posCurrentTask_g
 * points to. This behaviour is shown at the right side in the
 * diagram above.@n
 *
 * Note that interrupt service routines need some stack space to be
 * able to do their work - in the discussed configuration every ISR
 * would take some stack memory from the stack frame of the currently
 * active task. But this may be a problem at platforms that are low
 * on memory - it would be to expensive to increase every tasks stack
 * frame by the count of bytes an ISR would need. In this case, you can
 * set up a special stackframe that is only used by interrupt service
 * routines. The diagram below shows the small changes to the ISRs
 * discussed above. But attention - this method is only applicable on
 * platforms where interrupts can not interrupt each other.@n@n@n
 *
 * <p><img src="../pic/portfc3.png" align="middle" border=0></p>@n
 *
 * @{
 */

/* findbit  function or macro definition */
#if (DOX!=0) || (POSCFG_ROUNDROBIN == 0)
#ifdef FINDBIT
#define POS_FINDBIT(bf)          FINDBIT(bf, 0)
#define POS_FINDBIT_EX(bf, ofs)  FINDBIT(bf, 0)
#else  /* FINDBIT */
POSFROMEXT UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield);  /* arch_c.c */
#define POS_FINDBIT(bf)          p_pos_findbit(bf)
#define POS_FINDBIT_EX(bf, ofs)  p_pos_findbit(bf)
#endif /* FINDBIT */
#else  /* POSCFG_ROUNDROBIN */
#ifdef FINDBIT
#define POS_FINDBIT(bf)          FINDBIT(bf, 0)
#define POS_FINDBIT_EX(bf, ofs)  FINDBIT(bf, ofs)
#else  /* FINDBIT */
POSFROMEXT UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset);  /* arch_c.c */
#define POS_FINDBIT(bf)          p_pos_findbit(bf, 0)
#define POS_FINDBIT_EX(bf, ofs)  p_pos_findbit(bf, ofs)
#endif /* FINDBIT */
#endif /* POSCFG_ROUNDROBIN */

#if DOX!=0
/**
 * Bit finding function.
 * This function is called by the operating system to find the
 * first set bit in a bitfield. See the file fbit_gen.c for an example.
 * @param   bitfield  This is the bitfield that shall be scanned.
 * @return  the number of the first set bit (scanning begins with the lsb).
 * @note    ::POSCFG_ROUNDROBIN <b>must be defined to 0</b>
 *          to have this format of the function compiled in.
 */
POSFROMEXT UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield);  

/**
 * Bit finding function.
 * This function is called by the operating system to find the
 * first set bit in a bitfield. See the file fbit_gen.c for an example.
 * @param   bitfield  This is the bitfield that shall be scanned.
 * @param   rrOffset  Offset into the bitfield. Scanning begins here.
 * @return  the number of the first set bit (scanning begins with the lsb).
 * @note    ::POSCFG_ROUNDROBIN <b>must be defined to 1</b>
 *          to have this format of the function compiled in.
 */
POSFROMEXT UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset); 
#endif

#if (DOX!=0) || (POSCFG_CALLINITARCH != 0)
/**
 * Architecture port initialization.
 * This function is called from the ::posInit function to initialize
 * the architecture specific part of the operating system.
 * @note    ::POSCFG_CALLINITARCH must be defined to 1
 *          when ::posInit shall call this function.@n
 *          This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          A timer interrupt should be initialized in the funcion
 *          ::p_pos_startFirstContext.
 */
POSFROMEXT void POSCALL p_pos_initArch(void);
#endif

#if (DOX!=0) || (POSCFG_TASKSTACKTYPE == 0)
/**
 * Task initialization function.
 * This function is called by the operating system to
 * initialize the stack frame of a new task.
 * See the available port source files for an
 * example on how to write this function.
 * @param   task        pointer to the task environment structure.
 * @param   stackstart  pointer to the start of the stack memory.
 * @param   funcptr     pointer to the first function that shall
 *                      be executed by the new task.
 * @param   funcarg     argument that should be passed to the
 *                      first function.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 0</b>
 *          to have this format of the function compiled in.@n
 *          This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          The processor interrupts are disabled when this function
 *          is called.
 */
POSFROMEXT void POSCALL p_pos_initTask(POSTASK_t task, void *stackstart,
                                       POSTASKFUNC_t funcptr,
                                       void *funcarg);  /* arch_c.c */
#endif
#if (DOX!=0) || (POSCFG_TASKSTACKTYPE == 1)
/**
 * Task initialization function.
 * This function is called by the operating system to
 * initialize the stack frame of a new task.
 * See the available port source files for an
 * example on how to write this function.
 * @param   task        pointer to the task environment structure.
 * @param   stacksize   size of the stack memory for the new task.
 *                      The stack memory may be allocated
 *                      dynamically from within this function.
 * @param   funcptr     pointer to the first function that shall
 *                      be executed by the new task.
 * @param   funcarg     argument that should be passed to the
 *                      first function.
 * @return  zero on success. A negative value should be returned
 *          to denote an error (e.g. out of stack memory).
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 1</b>
 *          to have this format of the function compiled in.@n
 *          This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          The processor interrupts are disabled when this function
 *          is called.
 * @sa      p_pos_freeStack
 */
POSFROMEXT VAR_t POSCALL p_pos_initTask(POSTASK_t task, UINT_t stacksize,
                                POSTASKFUNC_t funcptr,
                                void *funcarg);  /* arch_c.c */
/**
 * Stack free function.
 * This function is called by the operating system to
 * free a stack frame that was set up by the function
 * ::p_pos_initTask.
 * See the available port source files for an
 * example on how to write this function.
 * @param   task  pointer to the task environment structure.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 1 or 2</b>
 *          to have this format of the function compiled in.@n
 *          This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          The processor interrupts are disabled when this function
 *          is called; but the processor may still write some bytes to
 *          the stack frame after this function was called and before
 *          the interrupts are enabled again.
 * @sa      p_pos_initTask
 */
POSFROMEXT void POSCALL p_pos_freeStack(POSTASK_t task);/* arch_c.c */
#endif

#if (DOX!=0) || (POSCFG_PORTMUTEX != 0)
/**
 * Port lock. In some really special cases it may be required that
 * Pico]OS must acquire a mutex before it can call the functions
 * ::p_pos_initTask and ::p_pos_freeStack. If you need such a mutex
 * for your port, please define ::POSCFG_PORTMUTEX to 1. Then implement
 * the both functions ::p_pos_lock and p_pos_unlock in your arch_c.c file.
 * @sa      p_pos_unlock, POSCFG_PORTMUTEX
 */
POSFROMEXT void POSCALL p_pos_lock(void);/* arch_c.c */

/**
 * Port unlock. Counterpart of function ::p_pos_lock.
 */
POSFROMEXT void POSCALL p_pos_unlock(void);/* arch_c.c */
#endif

#if (DOX!=0) || (POSCFG_TASKSTACKTYPE == 2)
/**
 * Task initialization function.
 * This function is called by the operating system to
 * initialize the stack frame of a new task.
 * This function is responsible to allocate the stack memory and
 * to store the pointer of the stack frame into the task environment.
 * See the available port source files for an
 * example on how to write this function.
 * @param   task        pointer to the task environment structure.
 * @param   funcptr     pointer to the first function that shall
 *                      be executed by the new task.
 * @param   funcarg     argument that should be passed to the
 *                      first function.
 * @return  zero on success. A negative value should be returned
 *          to denote an error (e.g. out of stack memory).
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 2</b>
 *          to have this format of the function compiled in.@n
 *          This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          The processor interrupts are disabled when this function
 *          is called.
 * @sa      p_pos_freeStack
 */
POSFROMEXT VAR_t POSCALL p_pos_initTask(POSTASK_t task, POSTASKFUNC_t funcptr,
                                        void *funcarg);  /* arch_c.c */

/**
 * Stack free function.
 * This function is called by the operating system to
 * free a stack frame that was set up by the function
 * ::p_pos_initTask.
 * See the available port source files for an
 * example on how to write this function.
 * @param   task  pointer to the task environment structure.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 1 or 2</b>
 *          to have this format of the function compiled in.@n
 *          This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          The processor interrupts are disabled when this function
 *          is called.
 * @sa      p_pos_initTask
 */
POSFROMEXT void POSCALL p_pos_freeStack(POSTASK_t task);/* arch_c.c */
#endif

/**
 * Context switch function.
 * This function is called by the operating system to
 * start the multitasking. The function has
 * to restore the first context from stack memory.
 * See the available port source files for an
 * example on how to write this function.
 * @note    This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          The processor interrupts are disabled when this function
 *          is called.
 * @sa      p_pos_softContextSwitch, p_pos_intContextSwitch
 */
POSFROMEXT void POSCALL p_pos_startFirstContext(void);   /* arch_c.c */

/**
 * Context switch function.
 * This function is called by the operating system to
 * initiate a software context switch. This function has then to
 * save all volatile processor registers to stack memory, switch
 * the context variable and restore the new context from
 * stack memory. See the available port source files for an
 * example on how to write this function.
 * @note    This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.@n
 *          The processor interrupts are disabled when this function
 *          is called.
 * @sa      p_pos_intContextSwitch, p_pos_startFirstContext
 */
POSFROMEXT void POSCALL p_pos_softContextSwitch(void);   /* arch_c.c */

/**
 * Context switch function.
 * This function is called by the operating system to initiate a
 * context switch from interrupt level. This function has then
 * to switch the context variable and restore the new context
 * from stack memory. See the available port source files for an
 * example on how to write this function.
 * @note    This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.
 *          The processor interrupts are disabled when this function
 *          is called.
 * @sa      p_pos_softContextSwitch, p_pos_startFirstContext
 */
POSFROMEXT void POSCALL p_pos_intContextSwitch(void);    /* arch_c.c */

#if (DOX!=0) || POSCFG_INT_EXIT_QUICK == 1

/**
 * Context switch function.
 * Called by c_pos_intExitQuick if task scheduling is needed.
 * This function should queue context switch somehow, 
 * for example in Arm Cortex-M CPU:s a PendSV exception can
 * be set pending.
 * @note    This function is not part of the pico]OS. It must be
 *          provided by the user, since it is architecture specific.
 *          The processor interrupts are disabled when this function
 *          is called.
 * @sa      p_pos_softContextSwitch, c_pos_intExitQuick
 */
POSFROMEXT void POSCALL p_pos_intContextSwitchPending(void);    /* arch_c.c */

#endif

/**
 * Interrupt control function.
 * This function must be called from an interrupt service routine
 * to show the operating system that an ISR is currently running.
 * This function must be called first before other operating system
 * functions can be called from within the ISR.
 * @sa      c_pos_intExit, c_pos_timerInterrupt
 */
POSEXTERN void POSCALL c_pos_intEnter(void);            /* picoos.c */

/**
 * Interrupt control function.
 * This function must be called from an interrupt service routine
 * to show the operating system that the ISR is going to complete its
 * work and no operating system functions will be called any more
 * from within the ISR.
 * @sa      c_pos_intEnter, c_pos_timerInterrupt
 */
POSEXTERN void POSCALL c_pos_intExit(void);             /* picoos.c */

#if (DOX!=0) || POSCFG_INT_EXIT_QUICK == 1
/**
 * Interrupt control function. Similar to c_pos_intExit, except
 * that context switch is not performed. Instead, a call to
 * p_pos_intContextSwitchQueue is performed if it is necessary
 * to perform task scheduling. p_pos_intContextSwitchQueue
 * should "queue" context switch to occur at later time.
 *
 * This was needed for Arm Cortex-M cpus, which need
 * to use PendSV exception for context switching.
 * @sa      c_pos_intExit, p_pos_intContextSwitchQueue
 */
POSEXTERN void POSCALL c_pos_intExitQuick(void);      /* picoos.c */

#endif

/**
 * Timer interrupt control function.
 * This function must be called periodically from within a timer
 * interrupt service routine. The whole system timing is derived
 * from this timer interrupt.@n
 *
 * A timer ISR could look like this:@n
 *
 * @code
 * PUSH ALL; // push all registers to stack@n
 *
 * if (posInInterrupt_g == 0)
 *   saveStackptrToCurrentTaskEnv();
 *
 * c_pos_intEnter();
 * c_pos_timerInterrupt();
 * c_pos_intExit();
 *
 * PULL ALL; // pull all registers from stack@n
 * RETI;     // return from interrupt@n
 * @endcode
 *
 * @note    Any other ISR looks like this, only the function
 *          ::c_pos_timerInterrupt is replaced by an user function.@n
 *          Dependent on the platform port, it can be necessary to
 *          evaluate the variable ::posRunning_g to ensure that the
 *          timer interrupt is not triggered when the OS is not yet
 *          running.@n
 *          To avoid this race condintions, it is better to initialize
 *          the timer interrupt in the function ::p_pos_startFirstContext.
 * @sa      c_pos_intEnter, c_pos_intExit
 */
POSEXTERN void POSCALL c_pos_timerInterrupt(void);      /* picoos.c */

/** @} */



/*---------------------------------------------------------------------------
 *  PROTOTYPES OF EXPORTED FUNCTIONS  (USER API)
 *-------------------------------------------------------------------------*/

/** @defgroup task Task Control Functions
 * @ingroup userapip
 * @{
 */

#if (DOX!=0) || (POSCFG_FEATURE_YIELD != 0)
/**
 * Task function.
 * This function can be called to give off processing time so other tasks
 * ready to run will be scheduled (= cooparative multitasking).
 * @note    ::POSCFG_FEATURE_YIELD must be defined to 1
 *          to have this function compiled in.
 * @sa      posTaskSleep
 */
POSEXTERN void POSCALL posTaskYield(void);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_SLEEP != 0)
/**
 * Task function.
 * Delay task execution for a couple of timer ticks.
 * @param   ticks  delay time in timer ticks
 *          (see ::HZ define and ::MS macro)
 * @note    ::POSCFG_FEATURE_SLEEP must be defined to 1
 *          to have this function compiled in.@n
 *          It is not guaranteed that the task will proceed
 *          execution exactly when the time has elapsed.
 *          A higher priorized task or a task having the same
 *          priority may steal the processing time.
 *          Sleeping a very short time is inaccurate.
 * @sa      posTaskYield, HZ, MS
 */
POSEXTERN void POSCALL posTaskSleep(UINT_t ticks);
#endif

#if (DOX!=0) || (POSCFG_TASKSTACKTYPE == 0)
/**
 * Task function.
 * Creates a new task. The stack memory is managed by the user.
 * @param   funcptr     pointer to the function that shall be executed
 *                      by the new task.
 * @param   funcarg     optional argument passed to function.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @param   stackstart  pointer to the stack memory for the new task.
 * @return  handle to the task. NULL is returned when the
 *          task could not be created.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 0</b>
 *          to have this format of the function compiled in.
 * @sa      posTaskExit
 */
POSEXTERN POSTASK_t POSCALL posTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                                          VAR_t priority, void *stackstart);

/**
 * Operating System Initialization.
 * This function initializes the operating system and starts the
 * first tasks: The idle task and the first user task.
 * @param   firstfunc   pointer to the first task function that
 *                      will run in the multitasking environment.
 * @param   funcarg     optional argument passed to the first task.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @param   stackFirstTask  pointer to the stack memory for the first task.
 * @param   stackIdleTask   pointer to the stack memory for the idle task.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 0</b>
 *          to have this format of the function compiled in.
 */
POSEXTERN void POSCALL posInit(POSTASKFUNC_t firstfunc, void *funcarg,
                               VAR_t priority,
                               void *stackFirstTask, void *stackIdleTask);
#endif
#if (DOX!=0) || (POSCFG_TASKSTACKTYPE == 1)
/**
 * Task function.
 * Creates a new task. The stack memory is managed by the achitecture
 * specific portion of the operating system, the size can be set by the user.
 * @param   funcptr     pointer to the function that shall be executed
 *                      by the new task.
 * @param   funcarg     optional argument passed to function.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @param   stacksize   size of the stack memory. This parameter is
 *                      passed to the architecture specific portion of
 *                      the operating system.
 * @return  handle to the task. NULL is returned when the
 *          task could not be created.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 1</b>
 *          to have this format of the function compiled in.
 * @sa      posTaskExit
 */
POSEXTERN POSTASK_t POSCALL posTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                                          VAR_t priority, UINT_t stacksize);

/**
 * Operating System Initialization.
 * This function initializes the operating system and starts the
 * first tasks: The idle task and the first user task.
 * @param   firstfunc   pointer to the first task function that
 *                      will run in the multitasking environment.
 * @param   funcarg     optional argument passed to the first task.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @param   taskStackSize  size of the stack memory for the first task.
 * @param   idleStackSize  size of the stack memory for the idle task.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 1</b>
 *          to have this format of the function compiled in.
 */
POSEXTERN void POSCALL posInit(POSTASKFUNC_t firstfunc, void *funcarg,
                               VAR_t priority, UINT_t taskStackSize,
                               UINT_t idleStackSize);
#endif
#if (DOX!=0) || (POSCFG_TASKSTACKTYPE == 2)
/**
 * Task function.
 * Creates a new task. The stack memory is fixed, its size is set by
 * the architecture specific portion of the operating system.
 * @param   funcptr     pointer to the function that shall be executed
 *                      by the new task.
 * @param   funcarg     optional argument passed to function.
 * @param   priority    task priority. Must be in the range
 *                      0.. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @return  handle to the task. NULL is returned when the
 *          task could not be created.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 2</b>
 *          to have this format of the function compiled in.
 * @sa      posTaskExit
 */
POSEXTERN POSTASK_t POSCALL posTaskCreate(POSTASKFUNC_t funcptr, void *funcarg,
                                          VAR_t priority);

/**
 * Operating System Initialization.
 * This function initializes the operating system and starts the
 * first tasks: The idle task and the first user task.
 * @param   firstfunc   pointer to the first task function that
 *                      will run in the multitasking environment.
 * @param   funcarg     optional argument passed to the first task.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @note    ::POSCFG_TASKSTACKTYPE <b>must be defined to 2</b>
 *          to have this format of the function compiled in.
 */
POSEXTERN void POSCALL posInit(POSTASKFUNC_t firstfunc, void *funcarg,
                               VAR_t priority);

#endif


#if (DOX!=0) || (POSCFG_FEATURE_EXIT != 0)
/**
 * Task function.
 * Terminate execution of a task.
 * @note    ::POSCFG_FEATURE_EXIT must be defined to 1 
 *          to have this function compiled in.
 * @sa      posTaskCreate
 */
POSEXTERN void POSCALL posTaskExit(void);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_GETTASK != 0)
/**
 * Task function.
 * Get the handle to the currently running task.
 * @return  the task handle.
 * @note    ::POSCFG_FEATURE_GETTASK must be defined to 1 
 *          to have this function compiled in.
 * @sa      posTaskCreate, posTaskSetPriority
 */
POSEXTERN POSTASK_t POSCALL posTaskGetCurrent(void);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_TASKUNUSED != 0)
/**
 * Task function.
 * Tests if a task is yet in use by the operating system.
 * This function can be used to test if a task has been
 * fully terminated (and the stack memory is no more in use).
 * @param   taskhandle  handle to the task.
 * @return  1 (=true) when the task is unused. If the task
 *          is still in use, zero is returned.
 *          A negative value is returned on error.
 * @note    ::POSCFG_FEATURE_TASKUNUSED must be defined to 1 
 *          to have this function compiled in.
 * @sa      posTaskCreate, posTaskExit
 */
POSEXTERN VAR_t POSCALL posTaskUnused(POSTASK_t taskhandle);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_SETPRIORITY != 0)
/**
 * Task function.
 * Change the priority of a task. Note that in a non-roundrobin
 * scheduling environment every priority level can only exist once.
 * @param   taskhandle  handle to the task.
 * @param   priority    new priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_SETPRIORITY must be defined to 1 
 *          to have this function compiled in.
 * @sa      posTaskGetPriority, posTaskGetCurrent, posTaskCreate
 */
POSEXTERN VAR_t POSCALL posTaskSetPriority(POSTASK_t taskhandle,
                                           VAR_t priority);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_GETPRIORITY != 0)
/**
 * Task function.
 * Get the priority of a task.
 * @param   taskhandle  handle to the task.
 * @return  the priority of the task. A negative value is returned on error.
 * @note    ::POSCFG_FEATURE_GETPRIORITY must be defined to 1 
 *          to have this function compiled in.
 * @sa      posTaskSetPriority, posTaskGetCurrent, posTaskCreate
 */
POSEXTERN VAR_t POSCALL posTaskGetPriority(POSTASK_t taskhandle);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_INHIBITSCHED != 0)
/**
 * Task function.
 * Locks the scheduler. When this function is called, no task switches
 * will be done any more, until the counterpart function ::posTaskSchedUnlock
 * is called. This function is usefull for short critical sections that
 * require exclusive access to variables. Note that interrupts still
 * remain enabled.
 * @note    ::POSCFG_FEATURE_INHIBITSCHED must be defined to 1 
 *          to have this function compiled in.
 * @sa      posTaskSchedUnlock
 */
POSEXTERN void POSCALL posTaskSchedLock(void);

/**
 * Task function.
 * Unlocks the scheduler. This function is called to leave a critical section.
 * If a context switch request is pending, the context switch will happen
 * directly after calling this function.
 * @note    ::POSCFG_FEATURE_INHIBITSCHED must be defined to 1 
 *          to have this function compiled in.
 * @sa      posTaskSchedLock
 */
POSEXTERN void POSCALL posTaskSchedUnlock(void);
#endif

#if (DOX!=0) || (POSCFG_TASKCB_USERSPACE > 0)
/**
 * Task function.
 * Returns a pointer to the user memory in the current task control block.
 * @note    ::POSCFG_TASKCB_USERSPACE must be defined to a nonzero value
 *          to have this function compiled in. ::POSCFG_TASKCB_USERSPACE
 *          is also used to set the size of the user memory (in bytes).
 * @return  pointer to user memory space.
 */
POSEXTERN void* POSCALL posTaskGetUserspace(void);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_IDLETASKHOOK != 0)
/**
 * Task function.
 * Install or remove an optional idle task hook function.
 * The hook function is called every time the system is idle.
 * It is possible to use this hook to implement your own idle task;
 * in this case the function does not need to return to the system.
 * You may insert a call to ::posTaskYield into your idle task loop
 * to get a better task performance.
 * @param   idlefunc  function pointer to the new idle task handler.
 *                    If this parameter is set to NULL, the idle
 *                    task function hook is removed again.
 * @return  This function may return a pointer to the last hook
 *          function set. If so (pointer is not NULL), the previous
 *          hook function should be called from within your
 *          idle task hook. This enables chaining of hook functions.
 * @note    ::POSCFG_FEATURE_IDLETASKHOOK must be defined to 1 
 *          to have this function compiled in.
 */
POSEXTERN POSIDLEFUNC_t POSCALL posInstallIdleTaskHook(POSIDLEFUNC_t idlefunc);
#endif

/** @} */

/*-------------------------------------------------------------------------*/

#if (DOX!=0) || (SYS_FEATURE_EVENTS != 0)
/** @defgroup sema Semaphore Functions
 * @ingroup userapip
 * Semaphores are basically used for task synchronization.
 * Task synchronization means that only a defined number of tasks can
 * execute a piece of code. Usually, a semaphore is initialized with
 * the value 1, so only one task can hold the semaphore at a time
 * (Please read the chapter about the mutex functions also if you
 * are interested in task synchronization).@n
 * The second thing semaphores can be used for is sending signals
 * to waiting tasks. Imagine you have an interrupt service routine
 * that is triggered every time when a big chunk of data is available
 * on a device. The data is to big to process them directly in the ISR.
 * The ISR will only trigger a semaphore (it will signalize the semaphore),
 * and a high priorized task waiting for the semaphore will be set to
 * running state and will process the data from the device. In this
 * case, the semaphore would be initialized with zero when it is created.
 * The first task requesting the semaphore would block immediately, and
 * can only proceed its work when the semaphore is triggered from outside.@n
 * 
 * Semaphores are implemented as counters. A task requesting a semaphore
 * (via ::posSemaGet or ::posSemaWait) will decrement the counter. If the
 * counter is zero, the task willing to decrement the counter is blocked.
 * When a semaphore is signaled (via ::posSemaSignal), the counter is
 * incremented. If the counter reaches a positive, nonzero value,
 * the highest priorized task pending on the semaphore is set to
 * running state and can decrement the counter by itself.
 * @{
 */

/**
 * Semaphore function.
 * Allocates a new semaphore object.
 * @param   initcount  Initial semaphore count
 *                     (see detailed semaphore description).
 * @return  the pointer to the new semaphore object. NULL is returned on error.
 * @note    ::POSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in.
 * @sa      posSemaDestroy, posSemaGet, posSemaWait, posSemaSignal
 */
POSEXTERN POSSEMA_t POSCALL posSemaCreate(INT_t initcount);

#if (DOX!=0) || (SYS_FEATURE_EVENTFREE != 0)
/**
 * Semaphore function.
 * Frees a no more needed semaphore object.
 * @param   sema  handle to the semaphore object.
 * @note    ::POSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in.@n
 *          ::POSCFG_FEATURE_SEMADESTROY must be defined to 1
 *          to have this function compiled in.
 * @sa      posSemaCreate
 */
POSEXTERN void POSCALL posSemaDestroy(POSSEMA_t sema);
#endif

/**
 * Semaphore function.
 * This function tries to get the semaphore object. If the semaphore
 * is in nonsignalized state (that means its counter is zero or less),
 * this function blocks the task execution until the semaphore
 * gets signaled.
 * @param   sema  handle to the semaphore object.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in.
 * @sa      posSemaWait, posSemaSignal, posSemaCreate
 */
#if (DOX!=0) || (POSCFG_SMALLCODE == 0) || (POSCFG_FEATURE_SEMAWAIT == 0)
POSEXTERN VAR_t POSCALL posSemaGet(POSSEMA_t sema);
#else
/* this define is for small code and it saves stack memory */
#define  posSemaGet(sema)  posSemaWait(sema, INFINITE)
#endif

/**
 * Semaphore function.
 * This function signalizes a semaphore object, that means it increments
 * the semaphore counter and sets tasks pending on the semaphore to 
 * running state, when the counter reaches a positive, nonzero value.
 * @param   sema  handle to the semaphore object.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in.
 * @sa      posSemaGet, posSemaWait, posSemaCreate
 */
POSEXTERN VAR_t POSCALL posSemaSignal(POSSEMA_t sema);

#if (DOX!=0) || (POSCFG_FEATURE_SEMAWAIT != 0)
/**
 * Semaphore function.
 * This function tries to get the semaphore object. If the semaphore
 * is in nonsignalized state (that means its counter is zero or less),
 * this function blocks the task execution until the semaphore
 * gets signaled or a timeout happens.
 * @param   sema  handle to the semaphore object.
 * @param   timeoutticks  timeout in timer ticks
 *          (see ::HZ define and ::MS macro).
 *          If this parameter is set to zero, the function immediately
 *          returns. If this parameter is set to INFINITE, the
 *          function will never time out.
 * @return  zero on success. A positive value (1 or TRUE) is returned
 *          when the timeout was reached.
 * @note    ::POSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in.@n
 *          ::POSCFG_FEATURE_SEMAWAIT must be defined to 1
 *          to have this function compiled in.
 * @sa      posSemaGet, posSemaSignal, posSemaCreate, HZ, MS
 */
POSEXTERN VAR_t POSCALL posSemaWait(POSSEMA_t sema, UINT_t timeoutticks);
#endif

#endif /* SYS_FEATURE_EVENTS */
/** @} */

/*-------------------------------------------------------------------------*/

#if (DOX!=0) || (POSCFG_FEATURE_MUTEXES != 0)
/** @defgroup mutex Mutex Functions
 * @ingroup userapip
 * Mutexes are used for task synchronization. A source code
 * area that is protected by a mutex can only be executed by
 * one task at the time. The mechanism is comparable with
 * a semaphore that is initialized with a counter of 1.
 * A mutex is a special semaphore, that allows a
 * task having the mutex locked can execute the mutex lock
 * functions again and again without being blocked
 * (this is called reentrancy).
 * @{
 */

/**
 * Mutex function.
 * Allocates a new mutex object.
 * @return  the pointer to the new mutex object. NULL is returned on error.
 * @note    ::POSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in.
 * @sa      posMutexDestroy, posMutexLock, posMutexTryLock, posMutexUnlock
 */
POSEXTERN POSMUTEX_t POSCALL posMutexCreate(void);

#if (DOX!=0) || (POSCFG_FEATURE_MUTEXDESTROY != 0)
/**
 * Mutex function.
 * Frees a no more needed mutex object.
 * @param   mutex  handle to the mutex object.
 * @note    ::POSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in.@n
 *          ::POSCFG_FEATURE_MUTEXDESTROY must be defined to 1
 *          to have this function compiled in.
 * @sa      posMutexCreate
 */
POSEXTERN void POSCALL posMutexDestroy(POSMUTEX_t mutex);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_MUTEXTRYLOCK != 0)
/**
 * Mutex function.
 * Tries to get the mutex lock. This function does not block when the
 * mutex is not available, instead it returns a value showing that
 * the mutex could not be locked.
 * @param   mutex  handle to the mutex object.
 * @return  zero when the mutex lock could be set. Otherwise, when
 *          the mutex lock is yet helt by an other task, the function
 *          returns 1. A negative value is returned on error.
 * @note    ::POSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in.@n
 *          ::POSCFG_FEATURE_MUTEXTRYLOCK must be defined to 1
 *          to have this function compiled in.
 * @sa      posMutexLock, posMutexUnlock, posMutexCreate
 */
POSEXTERN VAR_t POSCALL posMutexTryLock(POSMUTEX_t mutex);
#endif

/**
 * Mutex function.
 * This function locks a code section so that only one task can execute
 * the code at a time. If an other task already has the lock, the task
 * requesting the lock will be blocked until the mutex is unlocked again.
 * Note that a ::posMutexLock appears always in a pair with ::posMutexUnlock.
 * @param   mutex  handle to the mutex object.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in.
 * @sa      posMutexTryLock, posMutexUnlock, posMutexCreate
 */
POSEXTERN VAR_t POSCALL posMutexLock(POSMUTEX_t mutex);

/**
 * Mutex function.
 * This function unlocks a section of code so that other tasks
 * are able to execute it.
 * @param   mutex  handle to the mutex object.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in.
 * @sa      posMutexLock, posMutexTryLock, posMutexCreate
 */
POSEXTERN VAR_t POSCALL posMutexUnlock(POSMUTEX_t mutex);

#endif /* POSCFG_FEATURE_MUTEXES */
/** @} */

/*-------------------------------------------------------------------------*/

#if (DOX!=0) || (POSCFG_FEATURE_MSGBOXES != 0)
/** @defgroup msg Message Box Functions
 * @ingroup userapip
 * Message boxes are a mechanism that is used for inter-process or,
 * in the case of pico]OS, inter-task communication. All tasks
 * can post messages to each other, and the receiving task will
 * look into its message box and continues when it
 * got a new message. Note that a message box in the pico]OS
 * can hold chunk of messages, so that no message will be lost
 * while the receiving task is still busy processing the last
 * message. @n
 * There are two possible types of message boxes: The simple type
 * can only hold a pointer to a user supplied buffer. The other
 * message box type can hold whole messages with different sizes.
 * A message buffer must be allocated with posMessageAlloc by the
 * sending task, and the receiving task must free this buffer
 * again with ::posMessageFree. @n
 * To select the simple message box type, you have to set
 * the define ::POSCFG_MSG_MEMORY to 0. When you want to have the
 * full message buffer support, you must set ::POSCFG_MSG_MEMORY to 1.
 * @{
 */

#if (DOX!=0) || (POSCFG_MSG_MEMORY != 0)
/**
 * Message box function.
 * Allocates a new message buffer. The maximum buffer size is
 * set at compilation time by the define ::POSCFG_MSG_BUFSIZE.
 * Usually the sending task would allocate a new message buffer, fill
 * in its data and send it via ::posMessageSend to the receiving task.
 * @return  the pointer to the new buffer. NULL is returned on error.
 * @note    ::POSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.@n
 *          ::POSCFG_MSG_MEMORY must be defined to 1
 *          to have this function compiled in.
 * @sa      posMessageSend, posMessageGet, posMessageFree
 */
POSEXTERN void* POSCALL posMessageAlloc(void);

/**
 * Message box function.
 * Frees a message buffer again.
 * Usually the receiving task would call this function after
 * it has processed a message to free the message buffer again.
 * @param   buf  pointer to the message buffer that is no more used.
 * @note    ::POSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.@n
 *          ::POSCFG_MSG_MEMORY must be defined to 1
 *          to have this function compiled in.
 * @sa      posMessageGet, posMessageSend, posMessageAlloc
 */
POSEXTERN void POSCALL posMessageFree(void *buf);
#endif

/**
 * Message box function.
 * Sends a message to a task.
 * @param   buf  pointer to the message to send.
 *               If ::POSCFG_MSG_MEMORY is defined to 1,
 *               this pointer must point to the buffer
 *               that was allocated with ::posMessageAlloc.
 *               Note that this parameter must not be NULL.
 *               Exception: ::POSCFG_MSG_MEMORY = 0 and
 *               ::POSCFG_FEATURE_MSGWAIT = 0.
 * @param   taskhandle  handle to the task to send the message to.
 * @return  zero on success. When an error condition exist, a
 *          negative value is returned and the message buffer is freed.
 * @note    ::POSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.
 * @sa      posMessageAlloc, posMessageGet
 */
POSEXTERN VAR_t POSCALL posMessageSend(void *buf, POSTASK_t taskhandle);

/**
 * Message box function. Gets a new message from the message box.
 * If no message is available, the task blocks until a new message
 * is received.
 * @return  pointer to the received message. Note that the
 *          message memory must be freed again with ::posMessageFree
 *          when ::POSCFG_MSG_MEMORY is defined to 1. @n
 *          NULL may be returned when the system has not
 *          enough events. In this case, please increase the
 *          value for ::POSCFG_MAX_EVENTS in your poscfg.h .
 * @note    ::POSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.
 * @sa      posMessageFree, posMessageAvailable,
 *          posMessageWait, posMessageSend
 */
#if (DOX!=0) || (POSCFG_SMALLCODE == 0) || (POSCFG_FEATURE_MSGWAIT == 0)
POSEXTERN void* POSCALL posMessageGet(void);
#else
/* this define is for small code and it saves stack memory */
#define     posMessageGet()  posMessageWait(INFINITE)
#endif

/**
 * Message box function.
 * Tests if a new message is available
 * in the message box. This function can be used to prevent
 * the task from blocking.
 * @return  1 (=true) when a new message is available.
 *          Otherwise zero is returned. A negative value
 *          is returned on error.
 * @note    ::POSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.
 * @sa      posMessageGet, posMessageWait
 */
POSEXTERN VAR_t POSCALL posMessageAvailable(void);

#if (DOX!=0) || (POSCFG_FEATURE_MSGWAIT != 0)
/**
 * Message box function.
 * Gets a new message from the message box.
 * If no message is available, the task blocks until a new message
 * is received or the timeout has been reached.
 * @param   timeoutticks  timeout in timer ticks
 *          (see ::HZ define and ::MS macro).
 *          If this parameter is set to zero, the function immediately
 *          returns. If this parameter is set to INFINITE, the
 *          function will never time out.
 * @return  pointer to the received message. Note that the
 *          message memory must be freed again with posMessageFree
 *          when ::POSCFG_MSG_MEMORY is defined to 1.
 *          NULL is returned when no message was received
 *          within the specified time (=timeout). @n
 *          Hint: NULL may also be returned when the system has
 *          not enough events. In this case, please increase the
 *          value for ::POSCFG_MAX_EVENTS in your poscfg.h .
 * @note    ::POSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.@n
 *          ::POSCFG_FEATURE_MSGWAIT must be defined to 1
 *          to have this function compiled in.
 * @sa      posMessageFree, posMessageGet, posMessageAvailable,
 *          posMessageSend, HZ, MS
 */
POSEXTERN void* POSCALL posMessageWait(UINT_t timeoutticks);
#endif

#endif  /* POSCFG_FEATURE_MSGBOXES */
/** @} */

/*-------------------------------------------------------------------------*/

#if (DOX!=0) || (POSCFG_FEATURE_FLAGS != 0)
/** @defgroup flag Flag Functions
 * @ingroup userapip
 * Flags are one-bit semaphores. They can be used to simulate events.
 * A thread can simultaneousely wait for multiple flags to be set,
 * so it is possible to post multiple events to this thread.
 * The count of events a flag object can handle is only limited by the
 * underlaying architecutre, the maximum count is defined as ::MVAR_BITS - 1.
 * @{
 */
/**
 * Flag function.
 * Allocates a flag object. A flag object behaves like an array of
 * one bit semaphores. The object can hold up to ::MVAR_BITS - 1 flags.
 * The flags can be used to simulate events, so a single thread can wait
 * for several events simultaneously.
 * @return  handle to the new flag object. NULL is returned on error.
 * @note    ::POSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in.
 * @sa      posFlagGet, posFlagSet, posFlagDestroy
 */
POSEXTERN POSFLAG_t POSCALL posFlagCreate(void);

#if (DOX!=0) || (POSCFG_FEATURE_FLAGDESTROY != 0)
/**
 * Flag function.
 * Frees an unused flag object again.
 * @param   flg  handle to the flag object.
 * @note    ::POSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in.@n
 *          ::POSCFG_FEATURE_FLAGDESTROY must be defined to 1
 *          to have this function compiled in.
 * @sa      posFlagCreate
 */
POSEXTERN void POSCALL posFlagDestroy(POSFLAG_t flg);
#endif

/**
 * Flag function.
 * Sets a flag bit in the flag object and sets the task that
 * pends on the flag object to running state.
 * @param   flg     handle to the flag object.
 * @param   flgnum  Number of the flag to set. The flag number
 *                  must be in the range of 0 .. ::MVAR_BITS - 2.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in.
 * @sa      posFlagCreate, posFlagGet, posFlagWait
 */
POSEXTERN VAR_t POSCALL posFlagSet(POSFLAG_t flg, UVAR_t flgnum);

/**
 * Flag function.
 * Pends on a flag object and waits until one of the flags 
 * in the flag object is set.
 * @param   flg   handle to the flag object.
 * @param   mode  can be POSFLAG_MODE_GETSINGLE or POSFLAG_MODE_GETMASK.
 * @return  the number of the next flag that is set when mode is set
 *          to POSFLAG_MODE_GETSINGLE. When mode is set to 
 *          POSFLAG_MODE_GETMASK, a bit mask with all set flags is
 *          returned. A negative value is returned on error.
 * @note    ::POSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in.
 * @sa      posFlagCreate, posFlagSet, posFlagWait
 */
POSEXTERN VAR_t POSCALL posFlagGet(POSFLAG_t flg, UVAR_t mode);

#if (DOX!=0) || (POSCFG_FEATURE_FLAGWAIT != 0)
/**
 * Flag function.
 * Pends on a flag object and waits until one of the flags 
 * in the flag object is set or a timeout has happened.
 * @param   flg   handle to the flag object.
 * @param   timeoutticks  timeout in timer ticks
 *          (see ::HZ define and ::MS macro).
 *          If this parameter is set to zero, the function immediately
 *          returns. If this parameter is set to INFINITE, the
 *          function will never time out.
 * @return  a mask of all set flags (positive value).
 *          If zero is returned, the timeout was reached.
 *          A negative value denotes an error.
 * @note    ::POSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in.@n
 *          ::POSCFG_FEATURE_FLAGWAIT must be defined to 1
 *          to have this function compiled in.
 * @sa      posFlagCreate, posFlagSet, posFlagGet, HZ, MS
 */
POSEXTERN VAR_t POSCALL posFlagWait(POSFLAG_t flg, UINT_t timeoutticks);
#endif

#define POSFLAG_MODE_GETSINGLE   0
#define POSFLAG_MODE_GETMASK     1

#endif  /* POSCFG_FEATURE_FLAGS */
/** @} */

/*-------------------------------------------------------------------------*/

/** @defgroup timer Timer Functions
 * @ingroup userapip
 * A timer object is a counting variable that is counted down by the
 * system timer interrupt tick rate. If the variable reaches zero,
 * a semaphore, that is bound to the timer, will be signaled.
 * If the timer is in auto reload mode, the timer is restarted and
 * will signal the semaphore again and again, depending on the
 * period rate the timer is set to.
 * @{
 */
/** @def HZ
 * Hertz, timer ticks per second.
 * HZ is a define that is set to the number of ticks
 * the timer interrupt does in a second.
 * For example, the jiffies variable is incremented
 * HZ times per second. Also, much OS functions take
 * timeout parameters measured in timer ticks; thus
 * the HZ define can be taken as time base: HZ = 1 second,
 * 10*HZ = 10s, HZ/10 = 100ms, etc.
 * @sa jiffies, MS
 */
#if DOX!=0
#define HZ (timerticks per second)
#endif

/** @brief  Global timer variable.
 * The jiffies counter variable is incremented ::HZ times per second.
 *
 * The maximum count the jiffie counter can reach until it wraps around
 * is system dependent.
 * @note    ::POSCFG_FEATURE_JIFFIES must be defined to 1 
 *          to have jiffies support compiled in.
 * @sa HZ, POS_TIMEAFTER
 */
#if (DOX!=0) || (POSCFG_FEATURE_JIFFIES != 0)
#if (DOX!=0) || (POSCFG_FEATURE_LARGEJIFFIES == 0)
POSEXTERN  volatile JIF_t  jiffies;
#else
POSEXTERN  JIF_t POSCALL posGetJiffies(void);
#define jiffies  posGetJiffies()
#endif

/**
 * This macro is used to test if a specified time has expired.
 * It handles timer variable wrap arounds correctly.
 * The macro is used in conjunction with the jiffies variable,
 * the current jiffies should be passed as first parameter
 * to the macro. Example:@n
 * exptime = jiffies + HZ/2;@n
 * if (POS_TIMEAFTER(jiffies, exptime)) printf("500ms expired!\n");
 * @sa jiffies, HZ
 */
#define POS_TIMEAFTER(x, y)    ((((SJIF_t)(x)) - ((SJIF_t)(y))) >= 0)

#endif  /* POSCFG_FEATURE_JIFFIES */

#if (DOX!=0) || (POSCFG_FEATURE_TIMER != 0)

/**
 * Timer function.
 * Allocates a timer object. After a timer is allocated with this function,
 * it must be set up with posTimerSet and than started with posTimerStart.
 * @return  handle to the new timer object. NULL is returned on error.
 * @note    ::POSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in.
 * @sa      posTimerSet, posTimerStart, posTimerDestroy
 */
POSEXTERN POSTIMER_t POSCALL posTimerCreate(void);

/**
 * Timer function.
 * Sets up a timer object.
 * @param   tmr  handle to the timer object.
 * @param   sema seaphore object that shall be signaled when timer fires.
 * @param   waitticks  number of initial wait ticks. The timer fires the
 *                     first time when this ticks has been expired.
 * @param   periodticks  After the timer has fired, it is reloaded with
 *                       this value, and will fire again when this count
 *                       of ticks has been expired (auto reload mode).
 *                       If this value is set to zero, the timer
 *                       won't be restarted (= one shot mode).
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in.
 * @sa      posTimerCreate, posTimerStart
 */
POSEXTERN VAR_t POSCALL posTimerSet(POSTIMER_t tmr, POSSEMA_t sema,
                                    UINT_t waitticks, UINT_t periodticks);
/**
 * Timer function.
 * Starts a timer. The timer will fire first time when the
 * waitticks counter has been reached zero. If the periodticks
 * were set, the timer will be reloaded with this value.
 * @param   tmr  handle to the timer object.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in.
 * @sa      posTimerStop, posTimerFired
 */
POSEXTERN VAR_t POSCALL posTimerStart(POSTIMER_t tmr);

/**
 * Timer function.
 * Stops a timer. The timer will no more fire. The timer
 * can be reenabled with posTimerStart.
 * @param   tmr  handle to the timer object.
 * @return  zero on success.
 * @note    ::POSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in.
 * @sa      posTimerStart, posTimerDestroy
 */
POSEXTERN VAR_t POSCALL posTimerStop(POSTIMER_t tmr);

#if (DOX!=0) || (POSCFG_FEATURE_TIMERDESTROY != 0)
/**
 * Timer function.
 * Deletes a timer object and free its resources.
 * @param   tmr  handle to the timer object.
 * @note    ::POSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          ::POSCFG_FEATURE_TIMERDESTROY must be defined to 1
 *          to have this function compiled in.
 * @sa      posTimerCreate
 */
POSEXTERN void POSCALL posTimerDestroy(POSTIMER_t tmr);
#endif
#if (DOX!=0) || (POSCFG_FEATURE_TIMERFIRED != 0)
/**
 * Timer function.
 * The function is used to test if a timer has fired.
 * @param   tmr  handle to the timer object.
 * @return  1 when the timer has fired, otherwise 0.
 *          A negative value is returned on error.
 * @note    ::POSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          ::POSCFG_FEATURE_TIMERFIRED must be defined to 1
 *          to have this function compiled in.
 * @sa      posTimerCreate, posTimerSet, posTimerStart
 */
POSEXTERN VAR_t POSCALL posTimerFired(POSTIMER_t tmr);
#endif

#endif  /* POSCFG_FEATURE_TIMER */
/** @} */

/*-------------------------------------------------------------------------*/

#if (DOX!=0) || (POSCFG_FEATURE_SOFTINTS != 0)
/** @defgroup sint Software Interrupt Functions
 * @ingroup userapip
 * pico]OS has a built in mechanism to simulate software interrupts.
 * For example, software interrupts can be used to connect hardware
 * interrupts, that are outside the scope of pico]OS, to the realtime
 * operating system. A hardware interrupt will trigger a software
 * interrupt that can then signalize a semaphore object.@n
 * @n
 * Note that hardware interrupts, that do not call ::c_pos_intEnter and
 * ::c_pos_intExit, can't do calls to pico]OS functions, except to the
 * function ::posSoftInt.@n
 * @n
 * All software interrupts, that are triggered by a call to ::posSoftInt,
 * are chained into a global list. This list is then executed as soon
 * as possible, but at least when the pico]OS scheduler is called
 * (that is, for example, when a time slice has expired or a task
 * gives of processing time by itself).@n
 * @n
 * A software interrupt runs at interrupt level, that means with
 * interrupts disabled (pico]OS calls ::POS_SCHED_LOCK before executing
 * the software interrupt handler). The execution of software interrupt
 * handlers can not be inhibited by setting the ::posTaskSchedLock flag.
 * @n
 * Note that software interrupts need additional space on the
 * processors call stack. Make sure to have space for at least
 * 5 additional subroutine calls, plus the calls you will make in the
 * ISR handler routine.
 * @{
 */
/**
 * Software Interrupt Function.
 * Rises a software interrupt. The software interrupt handler will
 * be executed as soon as possible. See above for a detailed description.
 * @param   intno number of the interrupt to rise. Must be in the
 *          range of 0 to ::POSCFG_SOFTINTERRUPTS - 1.
 * @param   param optional parameter that will be passed to the
 *          software interrupt handler funtion.
 * @note    ::POSCFG_FEATURE_SOFTINTS must be defined to 1 
 *          to have software interrupt support compiled in.
 * @note    If a software interrupt is triggered from inside the
 *          pico]OS scope, usually from application level, it is
 *          recommended to do a call to ::posTaskYield after this
 *          function is called. This will immediately start the
 *          interrupt handler.
 * @sa      posSoftIntSetHandler, posSoftIntDelHandler, POSCFG_SOFTINTQUEUELEN
 */
POSEXTERN void POSCALL posSoftInt(UVAR_t intno, UVAR_t param);

/**
 * Software Interrupt Function.
 * Sets a software interrupt handler function. Before a software interrupt
 * can be rised by a call to ::posSoftInt, this function must be called
 * to tell pico]OS the handler function for the interrupt.
 * @param   intno number of the interrupt to rise. Must be in the
 *          range of 0 to ::POSCFG_SOFTINTERRUPTS - 1.
 * @param   inthandler pointer to the interrupt handler function.
 * @note    ::POSCFG_FEATURE_SOFTINTS must be defined to 1 
 *          to have software interrupt support compiled in.
 * @return  zero on success.
 * @sa      posSoftIntDelHandler, posSoftInt
 */
POSEXTERN VAR_t POSCALL posSoftIntSetHandler(UVAR_t intno,
                                             POSINTFUNC_t inthandler);

#if (DOX!=0) || (POSCFG_FEATURE_SOFTINTDEL != 0)
/**
 * Software Interrupt Function.
 * Deletes a software interrupt handler function, that was set
 * with ::posSoftIntSetHandler before.
 * @param   intno number of the interrupt to rise. Must be in the
 *          range of 0 to ::POSCFG_SOFTINTERRUPTS - 1.
 * @note    ::POSCFG_FEATURE_SOFTINTS must be defined to 1 
 *          to have software interrupt support compiled in.@n
 *          ::POSCFG_FEATURE_SOFTINTDEL must be defined to 1
 *          to have this function compiled in.
 * @return  zero on success.
 * @sa      posSoftIntDelHandler, posSoftInt
 */
POSEXTERN VAR_t POSCALL posSoftIntDelHandler(UVAR_t intno);
#endif

#endif  /* POSCFG_FEATURE_SOFTINTS */
/** @} */

/*-------------------------------------------------------------------------*/

#if (DOX!=0) || (POSCFG_FEATURE_ATOMICVAR != 0)
/** @defgroup atomic Atomic Variables
 * @ingroup userapip
 * Atomic variables are variables that can be accessed in an atomic manner,
 * that means a read-modify-write instruction is done in virtually one
 * single cycle. For example, the atomic access to a variable is necessary
 * when two tasks will do read-modify-write operations on a common
 * variable. Under usual circumstances you can ran into trouble when
 * a task that is just modifying the variable (that means it has read
 * and modified the variable but has not yet written the result back) is
 * interrupted by a second task that also modifies the variable. Thus the
 * modification the first task has done would be lost. Atomic variables
 * prevent this possible race condition. @n@n
 * pico]OS supports four functions to operate on atomic variables:
 * ::posAtomicSet, ::posAtomicGet, ::posAtomicAdd and ::posAtomicSub.
 * @{
 */
/**
 * Atomic Variable Function.
 * Sets an atomic variable to the specified value.
 * @param   var    pointer to the atomic variable that shall be set.
 * @param   value  the value the atomic variable shall be set to.
 * @note    ::POSCFG_FEATURE_ATOMICVAR must be defined to 1 
 *          to have atomic variable support compiled in.
 * @sa      posAtomicGet, posAtomicAdd, posAtomicSub
 */
POSEXTERN void POSCALL posAtomicSet(POSATOMIC_t *var, INT_t value);

/**
 * Atomic Variable Function.
 * Returns the current value of an atomic variable.
 * @param   var    pointer to the atomic variable which value
 *                 shall be read and returned.
 * @return  the value of the atomic variable.
 * @note    ::POSCFG_FEATURE_ATOMICVAR must be defined to 1 
 *          to have atomic variable support compiled in.
 * @sa      posAtomicSet, posAtomicAdd, posAtomicSub
 */
POSEXTERN INT_t POSCALL posAtomicGet(POSATOMIC_t *var);

/**
 * Atomic Variable Function.
 * Adds a value onto the current value of the atomic variable.
 * @param   var    pointer to the atomic variable.
 * @param   value  value that shall be added to the atomic variable.
 * @return  the content of the atomic variable before it was incremented.
 * @note    ::POSCFG_FEATURE_ATOMICVAR must be defined to 1 
 *          to have atomic variable support compiled in.
 * @sa      posAtomicSet, posAtomicGet, posAtomicSub
 */
POSEXTERN INT_t POSCALL posAtomicAdd(POSATOMIC_t *var, INT_t value);

/**
 * Atomic Variable Function.
 * Substracts a value from the current value of the atomic variable.
 * @param   var    pointer to the atomic variable.
 * @param   value  value that shall be substracted from the atomic variable.
 * @return  the content of the atomic variable before it was decremented.
 * @note    ::POSCFG_FEATURE_ATOMICVAR must be defined to 1 
 *          to have atomic variable support compiled in.
 * @sa      posAtomicSet, posAtomicGet, posAtomicAdd
 */
POSEXTERN INT_t POSCALL posAtomicSub(POSATOMIC_t *var, INT_t value);

#endif /* POSCFG_FEATURE_ATOMICVAR */
/** @} */


/*-------------------------------------------------------------------------*/

#if (DOX!=0) || (POSCFG_FEATURE_LISTS != 0)
/** @defgroup lists Lists
 * @ingroup userapip
 * Lists are multifunctional, often they are used for buffer queues or
 * other elements that need to be listed. pico]OS provides a set of
 * functions for managing nonblocking and blocking lists. @n
 * Nonblocking means that elements can be put to or taken from a list
 * without blocking the active task while an other task is also attempting
 * to access the list. This behaviour is very usefull for interrupt service
 * routines that need to send buffers through a queue to the
 * application task. @n
 * An example program that demonstrates the usage of lists is available
 * in the examples directory: lists.c
 * @{
 */

#define POSLIST_HEAD  0
#define POSLIST_TAIL  1

/**
 * List Function.
 * Adds an element to a list.
 * @param   listhead  pointer to the head of the list.
 * @param   pos       position where to add the element. Can be
 *                    POSLIST_HEAD to add the element to the head of
 *                    the list or POSLIST_TAIL to add the element to
 *                    the tail of the list.
 * @param   new       pointer to the list element to add.
 * @note    ::POSCFG_FEATURE_LISTS must be defined to 1 
 *          to have list support compiled in. @n
 *          Note that list heads must be initialized before elements
 *          can be added to the list.
 * @sa      posListGet, posListLen, posListRemove, posListJoin, posListInit
 */
POSEXTERN void POSCALL posListAdd(POSLISTHEAD_t *listhead, UVAR_t pos,
                                  POSLIST_t *new);

/**
 * List Function.
 * Takes an element from a list.
 * @param   listhead  pointer to the head of the list.
 * @param   pos       position where to take the element from. Can be
 *                    POSLIST_HEAD to take the element from the head of
 *                    the list or POSLIST_TAIL to take the element from
 *                    the tail of the list.
 * @param   timeout   If timeout is set to zero, the function does not
 *                    wait for a new list element when the list is empty
 *                    (poll mode).
 *                    If timeout is set to INFINITE, the function waits
 *                    infinite (without timeout) for a new list element
 *                    to arrive. Any other value describes a timeout
 *                    in timerticks (see ::HZ and ::MS ). If the list
 *                    is still empty after the timeout ticks expired,
 *                    the function returns a NULL pointer.
 * @return  On success, this function returns the pointer to the
 *          element and the element is removed from the list. The
 *          function returns a NULL pointer when the list is empty
 *          (timeout == 0) or the timeout has expired (timeout != 0).
 * @note    ::POSCFG_FEATURE_LISTS must be defined to 1 
 *          to have list support compiled in. @n
 *          To be able to wait with timeout (timeout is set to nonzero and
 *          is not equal to INFINITE), the feature ::POSCFG_FEATURE_SEMAWAIT
 *          must be enabled. Note that only one task per time can wait
 *          for a new list element (timeout != 0). If multiple tasks attempt
 *          to wait with or without timeout for the same list, the behaviour
 *          of this function is undefined.
 * @sa      posListAdd, posListLen, posListRemove, posListJoin, posListInit
 */
POSEXTERN POSLIST_t* POSCALL posListGet(POSLISTHEAD_t *listhead, UVAR_t pos,
                                        UINT_t timeout);

/**
 * List Function.
 * Removes an element from a list.
 * @param   listelem  pointer to the element to remove.
 * @note    ::POSCFG_FEATURE_LISTS must be defined to 1 
 *          to have list support compiled in.
 * @sa      posListAdd, posListGet, posListLen, posListJoin, posListInit
 */
POSEXTERN void POSCALL posListRemove(POSLIST_t *listelem);

#if (DOX!=0) || (POSCFG_FEATURE_LISTJOIN != 0)
/**
 * List Function.
 * Joins two lists together. The elements contained in the joinlist 
 * are moved to the baselist. After this operation the joinlist is empty.
 * @param   baselisthead  pointer to the head of the list that shall
 *                        receive the elements of the second list.
 * @param   pos           position where the elements of the other list
 *                        shall be inserted. Can be POSLIST_HEAD to
 *                        insert the elements at the head of the
 *                        baselist or POSLIST_TAIL to insert the
 *                        elements at the tail of the baselist.
 * @param   joinlisthead  pointer to the list which contents shall be
 *                        moved to the baselist.
 * @note    ::POSCFG_FEATURE_LISTS must be defined to 1 
 *          to have list support compiled in. @n
 *          ::POSCFG_FEATURE_LISTJOIN must be defined to 1
 *          to have this function compiled in.
 * @sa      posListAdd, posListGet, posListJoin, posListInit
 */
POSEXTERN void POSCALL posListJoin(POSLISTHEAD_t *baselisthead, UVAR_t pos,
                                   POSLISTHEAD_t *joinlisthead);
#endif

#if (DOX!=0) || (POSCFG_FEATURE_LISTLEN != 0)
/**
 * List Function.
 * Returns the length of a list.
 * @param   listhead  pointer to the head of the list.
 * @return  the length of the list
 * @note    ::POSCFG_FEATURE_LISTS must be defined to 1 
 *          to have list support compiled in. @n
 *          ::POSCFG_FEATURE_LISTLEN must be defined to 1
 *          to have this function compiled in.
 * @sa      posListAdd, posListGet, posListRemove
 */
POSEXTERN UINT_t POSCALL posListLen(POSLISTHEAD_t *listhead);
#endif

/**
 * List Function.
 * Initializes the head of a list. This function must be called first
 * before elements can be added to the list.
 * @param   listhead    pointer to the listhead to initialize.
 * @note    ::POSCFG_FEATURE_LISTS must be defined to 1 
 *          to have list support compiled in. @n
 *          If a list is no more used, the function ::posListTerm
 *          should be called to free operating system resources.
 * @sa      posListTerm, posListAdd, posListGet
 */
POSEXTERN void POSCALL posListInit(POSLISTHEAD_t *listhead);

/**
 * List Function.
 * Frees operating system resources when a list is no more needed.
 * @param   listhead    pointer to the head of the list.
 * @note    ::POSCFG_FEATURE_LISTS must be defined to 1 
 *          to have list support compiled in.
 */
POSEXTERN void POSCALL posListTerm(POSLISTHEAD_t *listhead);

#if (DOX!=0)
/**
 * List Macro.
 * This macro enables the access to the data structure that is
 * linked with a list element.
 * @param   elem    pointer to the list element of type ::POSLIST_t
 * @param   type    type of the data structure where ::POSLIST_t is
 *                  an element from.
 * @param   member  the member name of the list element ::POSLIST_t
 *                  in the structure @e type.
 * @returns a pointer to the data structure where the list element
 *          is a member from.
 */
#define POSLIST_ELEMENT(elem, type, member)
#else
#define POSLIST_ELEMENT(elem, type, member) \
        ((type*)((char*)(elem)-(char*)(&((type*)NULL)->member)))
#endif

/** List Macro. Tests if a list is empty.
 * Pay attention to task synchronization when using this macro.
 */
#define POSLIST_IS_EMPTY(plisthead) \
          ((plisthead)->next == (POSLIST_t*)(plisthead))

/** List Macro. Returns a pointer to the next element in a list.
 * Pay attention to task synchronization when using this macro.
 */
#define POSLIST_NEXT_ENTRY(plist)               (plist)->next

/** List Macro. Returns a pointer to the previous element in a list.
 * Pay attention to task synchronization when using this macro.
 */
#define POSLIST_PREV_ENTRY(plist)               (plist)->prev

/** List Macro. Returns a pointer to the first entry of a list.
 * Pay attention to task synchronization when using this macro.
 */
#define POSLIST_FIRST_ENTRY(plisthead)          (plisthead)->next

/** List Macro. Returns a pointer to the last element of a list.
 * Pay attention to task synchronization when using this macro.
 */
#define POSLIST_LAST_ENTRY(plisthead)           (plisthead)->prev

/** List Macro. Tests if an element is the first one in a list.
 * Pay attention to task synchronization when using this macro.
 */
#define POSLIST_IS_FIRST_ENTRY(element) ((element)->prev==(element)->head)

/** List Macro. Tests if an element is the last one in a list.
 * Pay attention to task synchronization when using this macro.
 */
#define POSLIST_IS_LAST_ENTRY(element)  ((element)->next==(element)->head)

/** List Macro. Tests if the end of a list is reached when using a for-loop.
 */
#define POSLIST_IS_END(plisthead, element) \
          ((element)==(POSLIST_t*)(plisthead))


#if (DOX!=0)
/**
 * List Macro.
 * This macro expands to a for-loop that walks over all list entries
 * in the specified list. The body of the for-loop must be enclosured
 * in braces { }.
 * @param   plisthead   pointer to the head of the list
 * @param   runvar      run variable of type ::POSLIST_t. This variable
 *                      is the index variable in the for-loop.
 * @note    When using this macro you must pay attention about task
 *          synchronization. You may need to protect all list operations by
 *          a semaphore to ensure list integrity while executing this loop.
 * @note    It is not allowed to take an element from the list while
 *          being in the loop. But if you plan such an operation,
 *          please see the defintion of the macros
 *          ::POSLIST_FOREACH_BEGIN and ::POSLIST_FOREACH_END.
 */
#define POSLIST_FOR_EACH_ENTRY(plisthead, runvar)
#else
#define POSLIST_FOR_EACH_ENTRY(plisthead, runvar) \
  for (runvar = POSLIST_FIRST_ENTRY(plisthead);   \
       !POSLIST_IS_END(plisthead, runvar);        \
       runvar = POSLIST_NEXT_ENTRY(runvar))
#endif

#if (DOX!=0)
/**
 * List Macro.
 * This macro expands to a complex for-loop that walks over all list entries
 * in the specified list. This macro allows complex operations on the list
 * while being in the loop, and it simplifies the access to the data
 * structures that are linked to the list elements.
 * @param   plisthead   pointer to the head of the list
 * @param   runvar      run variable of the type your data structure is.
 *                      Note that this variable must be a structure pointer.
 *                      This variable is the index variable in the for-loop.
 * @param   type        type of your data structure where ::POSLIST_t is
 *                      an element from.
 * @param   listmember  the member name of the list element ::POSLIST_t
 *                      in your data structure @e type.
 * @note    When using this macro you must pay attention about task
 *          synchronization. You may need to protect all list operations by
 *          a semaphore to ensure list integrity while executing this loop.
 * @note    The end of the loop must be marked by the macro
 *          ::POSLIST_FOREACH_END.
 * @sa      POSLIST_FOREACH_END
 */
#define POSLIST_FOREACH_BEGIN(plisthead, runvar, type, listmember)
#else
#define POSLIST_FOREACH_BEGIN(plisthead, runvar, type, listmember) \
  do { POSLIST_t *r, *n; \
    for (r = POSLIST_FIRST_ENTRY(plisthead); \
         !POSLIST_IS_END(plisthead, r); r = n) { \
         n = POSLIST_NEXT_ENTRY(r); \
      runvar = POSLIST_ELEMENT(r, type, listmember);
#endif

#if (DOX!=0)
/**
 * List Macro.
 * This macro is the counterpart to ::POSLIST_FOREACH_BEGIN and must
 * be placed at the end of a for-loop that was initiated with
 * ::POSLIST_FOREACH_BEGIN.
 */
#define POSLIST_FOREACH_END
#else
#define POSLIST_FOREACH_END \
  } } while(0)
#endif

#endif /* POSCFG_FEATURE_LISTS */
/** @} */


/*---------------------------------------------------------------------------
 *  DEBUG FEATURES
 *-------------------------------------------------------------------------*/

/** @defgroup debug Debug Features
 * @ingroup intro
 *
 * Sometimes it is really hard to debug multitasking applications.
 * Pico]OS supports you by providing assess to some helpful informations.
 * In conjunction with an incircuit-debugger with the appropriated debugger
 * IDE you will have a powerfull tool to debug your applications. @n
 *
 * You need to enable the internal pico]OS debugging support by setting
 * the define ::POSCFG_FEATURE_DEBUGHELP to 1. It is also required that
 * you compile the code with debug flags set, and the global preprocessor
 * define _DBG must be set. @n
 *
 * Pico]OS exports some variables that are helpful for debugging: @n
 *
 * - ::picodeb_tasklist   points to the list of all currently started tasks@n
 * - ::picodeb_eventlist  points to the list of all used events (semaphores,
 *                        mutexes, flag events etc.) @n
 * - ::picodeb_taskhistory  is a history of the last 3 tasks that run @n
 * - ::posCurrentTask_g   contains the handle to the currently running task@n
 * - ::posInInterrupt_g   is nonzero when pico]OS executes an interupt @n
 *
 * @n@b Hint: @n
 * The global variables ::picodeb_tasklist and ::picodeb_eventlist are
 * pointing to double-chained lists. This lists contain helpful informations
 * about the state of tasks and events. When you observe that some task of
 * your application hangs and you don't know why, you can search the
 * task-list for the appropriated task (simply follow as often as necessary
 * the 'next' pointer in the structure until you have found the task) and
 * then have a look to the tasks state. If the task pends on an event, you
 * can follow the event reference pointer to observe the current state of
 * the event. Some IDE do have a kind of "OS-awareness". The both variables
 * ::picodeb_tasklist and ::picodeb_eventlist should make it possible to
 * teach your IDE the pico]OS. Please have a look into the debuggers
 * documentation for how to integrate OS-awareness into your debugger. @n
 *
 * If you consider to use the internal pico]OS debug feature, you should
 * name your tasks and events. This simplifies the search for tasks and
 * events in the global lists. Pico]OS provides two macros for doing this:
 * ::POS_SETTASKNAME and ::POS_SETEVENTNAME.
 * @{
 */


#if (DOX!=0) || defined(HAVE_PLATFORM_ASSERT)
/** @brief pico]OS assertion macro.
 *
 * For testing and debugging you should enable pico]OS internal assertions.
 * To do so, you must set the define HAVE_PLATFORM_ASSERT in your port.h
 * -file and your platform port must provide this function:  void
 * p_pos_assert(const char* text, const char *filename, int linenumber).
 * The function gets called every time pico]OS has failed an assertion.
 */
#define P_ASSERT(text,x) \
  if (!(x)) POSCALL p_pos_assert((const char*)(text), __FILE__, __LINE__)
#else
#define P_ASSERT(text,x)  do { } while(0)
#endif

#if (DOX!=0) || defined(POS_DEBUGHELP)

/** @brief  Task states
 * (used for debugging when ::POSCFG_FEATURE_DEBUGHELP is set to 1)
 * @sa POSCFG_FEATURE_DEBUGHELP
 */
enum PTASKSTATE
{
  task_notExisting = 0,  /*!< 0: Task does not exist. */
  task_created     = 1,  /*!< 1: Task was created but did not run yet. */
  task_running     = 2,  /*!< 2: Task is currently running. */
  task_suspended   = 3,  /*!< 3: Task was suspended. */
  task_sleeping    = 4,  /*!< 4: Task is sleeping for a period of time. */
  task_waitingForSemaphore = 5, /*!< 5: Task is waiting for a semaphore.*/
  task_waitingForSemaphoreWithTimeout = 6, /*!< 6: Task is waiting for a
                           semaphore, with timeout. */
  task_waitingForMutex = 7, /*!< 7: Task is waiting for a mutex. */
  task_waitingForMutexWithTimeout = 8,  /*!< 8: Task is waiting for a
                           mutex, with timeout. */
  task_waitingForFlag  = 9, /*!< 9: Task is waiting for a flag event. */
  task_waitingForFlagWithTimeout = 10, /*!< 10: Task is waiting for a
                           flag event, with timeout. */
  task_waitingForMessage = 11, /*!< 11: Task is waiting for a message. */
  task_waitingForMessageWithTimeout = 12   /*!< 12: Task is waiting for a
                           message, with timeout. */
};
typedef enum PTASKSTATE PTASKSTATE;

/** @brief  Event types
 * (used for debugging when ::POSCFG_FEATURE_DEBUGHELP is set to 1)
 * @sa POSCFG_FEATURE_DEBUGHELP
 */
enum PEVENTTYPE
{
  event_semaphore = 0,  /*!< 0: The event object is a semaphore. */
  event_mutex     = 1,  /*!< 1: The event object is a mutex. */
  event_flags     = 2   /*!< 2: The event object is a flags field. */
};
typedef enum PEVENTTYPE PEVENTTYPE;

/** @brief Event info structure.
 *
 * This structure can be used by a debugger IDE to display
 * event status information.
 * (used for debugging when ::POSCFG_FEATURE_DEBUGHELP is set to 1)
 * @note  This structure must never be changed to keep
 *         compatibility with existing debugger integrations.
 * @sa picodeb_eventlist, PICOTASK
 */
typedef struct PICOEVENT
{
  struct PICOEVENT  *next;  /*!< @brief
                                 Pointer to the next event info structure. */
  struct PICOEVENT  *prev;  /*!< @brief
                                 Pointer to the previous event structure. */
#if DOX
  PEVENTTYPE        type;   /*!< @brief
                                 Type of the event (semaphore / mutex /
                                 flag event, see PEVENTTYPE for details). */
#else
  enum PEVENTTYPE   type;
#endif
  void              *handle;/*!< @brief
                                 Handle value of the pico]OS event. */ 
  const char        *name;  /*!< @brief
                                 Name of this event (ASCII string) */
  INT_t             counter;/*!< @brief
                                 State of the internal counter of
                                 the event (semaphore or mutex), or
                                 bitfield of flags (flag event) */
} PICOEVENT;

/** @brief Task info structure.
 * 
 * This structure can be used by a debugger IDE to display
 * task status information.
 * (used for debugging when ::POSCFG_FEATURE_DEBUGHELP is set to 1)
 * @note  This structure must never be changed to keep
 *        compatibility with existing debugger integrations.
 * @sa picodeb_tasklist, PICOEVENT
 */
typedef struct PICOTASK
{
  struct PICOTASK   *next;  /*!< @brief
                                 Pointer to the next task info structure. */
  struct PICOTASK   *prev;  /*!< @brief
                                 Pointer to the previous task structure. */
  POSTASK_t         handle; /*!< @brief
                                 Handle value of the pico]OS task. */ 
  POSTASKFUNC_t     func;   /*!< @brief
                                 First function that ran in the task context.*/
  const char        *name;  /*!< @brief
                                 Name of this task (ASCII string) */
#if DOX
  PTASKSTATE        state;  /*!< @brief
                                 Current state of the task
                                 (see PTASKSTATE for details).*/
#else
  enum PTASKSTATE   state;
#endif
  struct PICOEVENT  *event; /*!< @brief
                                 Cross-reference to the involved event.*/
  UINT_t            timeout;/*!< @brief
                                 State of the task's timeout counter. */
} PICOTASK;

#if DOX
/** @brief  This macro assigns a name to a pico]OS task.
 * 
 * This is usefull when you are debugging your application by
 * using the global tasklist that is referenced by the variable
 * ::picodeb_tasklist. It is easier to navigate through
 * the list when the tasks have names. Examples:
 *
 * @code
 *   POSTASK_t htask;
 *   htask = posTaskCreate(workertaskfunc, NULL, 2);
 *   POS_SETTASKNAME(htask, "worker task");
 * @endcode
 *
 * @code
 *   POS_SETTASKNAME(posGetCurrentTask(), "receiver task");
 * @endcode
 *
 * @note It is not necessary to use this macro when you are
 *       using the function ::nosTaskCreate to start a new task.
 *   
 * @sa POS_SETEVENTNAME
 */
#define POS_SETTASKNAME(taskhandle, taskname)

/** @brief  This macro assigns a name to a pico]OS event.
 *
 * You can use this macro to assign a name to a pico]OS events,
 * such as semaphores, mutextes and flag events.
 * This is usefull when you are debugging your application by
 * using the global eventlist that is referenced by the variable
 * ::picodeb_eventlist. It is easier to navigate through
 * the list when the events have names. Example:
 *
 * @code
 *   POSSEMA_t sem;
 *   sem = posSemaCreate(0);
 *   POS_SETEVENTNAME(sem, "timer semaphore");
 * @endcode
 *
 * @note If you are using nano-layer functions to create semaphores,
 *       mutexes and flag events, you do not need this macro.
 * @sa POS_SETTASKNAME
 */
#define POS_SETEVENTNAME(eventhandle, name)

#else
#define POS_SETTASKNAME(taskhandle, taskname)  \
  do { if ((taskhandle) != NULL) (taskhandle)->deb.name = taskname;} while(0)
#define POS_SETEVENTNAME(eventhandle, name) \
  posdeb_setEventName(eventhandle, name)
POSEXTERN void posdeb_setEventName(void *event, const char *name);
#endif

#ifdef _POSCORE_C
struct PICOTASK  *picodeb_taskhistory[3];
struct PICOTASK  *picodeb_tasklist = NULL;
struct PICOEVENT *picodeb_eventlist = NULL;
#else

/** @brief  This array contains the last 3 tasks that run.
 *
 * This array can be used for debugging. The array has 3 entries,
 * each is pointing to a task debug structure. This array is a history
 * of the last 3 tasks that has run, and array element 0 is the task
 * that currently runs, element 1 points to the task before, and so on.
 * @note  A NULL pointer in the task history means a hardware interrupt.
 * @note  You can use the macro ::POS_SETTASKNAME to assign a name
 *        to a task. @n
 *        ::POSCFG_FEATURE_DEBUGHELP must be defined to 1 to enable
 *        debug support
 * @sa picodeb_tasklist, picodeb_eventlist
 */
extern struct PICOTASK  *picodeb_taskhistory[3];

/** @brief  Pointer to the list of active tasks.
 *
 * This variable can be used for debugging. It points to a list of
 * all currently active (=created) tasks in the system. The list is
 * double-chained with next- and prev- pointers. See the description
 * of the ::PICOTASK structure for details.
 * @note  You can use the macro ::POS_SETTASKNAME to assign a name
 *        to a task. @n
 *        ::POSCFG_FEATURE_DEBUGHELP must be defined to 1 to enable
 *        debug support
 * @sa picodeb_eventlist, picodeb_taskhistory
 */
extern struct PICOTASK  *picodeb_tasklist;

/** @brief  Pointer to the list of all system events.
 *
 * This variable can be used for debugging. It points to a list of
 * all active events in the system. An event can be a semaphore,
 * a mutex or a flag object. To differentiate the events, there is
 * a type-field in the ::PICOEVENT structure.
 * The list is double-chained with next- and prev- pointers.
 * See the description of the ::PICOEVENT structure for details.
 * @note  You can use the macro ::POS_SETEVENTNAME to assign a name
 *        to an event.
 * @note  ::POSCFG_FEATURE_DEBUGHELP must be defined to 1 to enable
 *        debug support
 * @sa picodeb_tasklist, picodeb_taskhistory
 */
extern struct PICOEVENT *picodeb_eventlist;
#endif

#else /* POS_DEBUGHELP */

#define POS_SETTASKNAME(taskhandle, name)  do { } while(0)
#define POS_SETEVENTNAME(eventhandle, name)  do { } while(0)

#endif /* POS_DEBUGHELP */
/** @} */


/* ==== END OF USER API ==== */



/*---------------------------------------------------------------------------
 *  INTERNAL DEFINITIONS
 *-------------------------------------------------------------------------*/

#ifdef PICOS_PRIVINCL

#if MVAR_BITS == 8
#ifndef _POSCORE_C
POSEXTERN UVAR_t posShift1lTab_g[8];
#endif
#define pos_shift1l(bits)   posShift1lTab_g[(UVAR_t)(bits)]
#else
#define pos_shift1l(bits)   (((UVAR_t)1)<<(bits))
#endif

#endif /* PICOS_PRIV_H */

/*-------------------------------------------------------------------------*/

/* include the header of the nano layer */

#ifdef POSNANO
#ifdef POSCFG_ENABLE_NANO
#undef POSCFG_ENABLE_NANO
#endif
#define POSCFG_ENABLE_NANO  1
#else
#ifndef POSCFG_ENABLE_NANO
#define POSCFG_ENABLE_NANO  0
#endif
#endif
#if POSCFG_ENABLE_NANO != 0
#ifdef PICOSUBDIR
/* required because of stupid CC65 compiler */
#include <picoos\pos_nano.h>
#else
#include <pos_nano.h>
#endif
#endif

/*-------------------------------------------------------------------------*/

/* defaults for hooks */

#ifndef POSCFG_TASKEXIT_HOOK
#define POSCFG_TASKEXIT_HOOK  0
#endif

/*-------------------------------------------------------------------------*/

/* fully define the task structure */

#ifndef NOS_TASKDATA
#define NOS_TASKDATA
#endif

#if POSCFG_TASKEXIT_HOOK != 0
typedef enum {
 texh_exitcalled,
 texh_freestackmem
} texhookevent_t;
typedef void (*POSTASKEXITFUNC_t)(POSTASK_t task, texhookevent_t event);
#endif

struct POSTASK {
    POS_USERTASKDATA
    NOS_TASKDATA
#if POSCFG_TASKCB_USERSPACE > 0
    UVAR_t      usrspace[(POSCFG_TASKCB_USERSPACE + POSCFG_ALIGNMENT +
                         sizeof(UVAR_t)-2) / sizeof(UVAR_t)];
#endif
#if POSCFG_TASKEXIT_HOOK != 0
    POSTASKEXITFUNC_t  exithook;
#endif
#if DOX==0
#if POSCFG_ARGCHECK > 1
    UVAR_t      magic;
#endif
#if SYS_TASKDOUBLELINK != 0
    struct POSTASK  *prev;
#endif
    struct POSTASK  *next;
    UVAR_t      bit_x;
#if SYS_TASKTABSIZE_Y > 1
    UVAR_t      bit_y;
    UVAR_t      idx_y;
#endif
#ifndef POS_DEBUGHELP
    UINT_t      ticks;
#endif
#if SYS_TASKSTATE != 0
    UVAR_t      state;
#endif
#if POSCFG_FEATURE_ERRNO != 0
    VAR_t       error;
#endif
#if POSCFG_FEATURE_MSGBOXES != 0
    UVAR_t      msgwait;
    POSSEMA_t   msgsem;
    void        *firstmsg;
    void        *lastmsg;
#endif
#ifdef POS_DEBUGHELP
    struct PICOTASK  deb;
#endif
#if SYS_TASKEVENTLINK != 0
    void        *event;
#endif
#endif /* !DOX */
};


#endif /* _PICOOS_H */

