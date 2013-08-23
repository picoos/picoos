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
 * @file    poscfg.h
 * @brief   pico]OS configuration file
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: poscfg.h,v 1.13 2006/03/14 18:39:27 dkuschel Exp $
 */


#ifndef _POSCFG_H
#define _POSCFG_H


/*---------------------------------------------------------------------------
 *  OS CORE SETTINGS
 *-------------------------------------------------------------------------*/

/** @defgroup coreset OS Core Settings
 * @ingroup configp
 * @{
 */

/** Maximum count of priority levels.
 * This define limits the maximum count of available priority levels.
 * For the round robin scheduler, the maximum count is equal to ::MVAR_BITS.
 * For the standard scheduler, the maximum count cannot exceed ::MVAR_BITS ^ 2.
 */
#define POSCFG_MAX_PRIO_LEVEL    8

/** Maximum number of allowed tasks per priority level.
 * If the standard scheduler is used, this define automatically
 * defaults to 1, since the standard scheduler supports only
 * one task at each priority. If the round robin scheduling scheme
 * is used (the define ::POSCFG_ROUNDROBIN is set to 1), this define
 * limits the count of tasks that can be at the same priority.
 * The value for this definition must be a power of two and can
 * not exceed the count of bits set by ::MVAR_BITS.
 */
#define POSCFG_TASKS_PER_PRIO    8 

/** Maximum count of tasks.
 * This define sets the maximum count of task data structures which can be
 * allocated. Even if the define ::POSCFG_MAX_PRIO_LEVEL allows in conjunction
 * with ::POSCFG_TASKS_PER_PRIO more tasks, the maximum count of usable tasks
 * is limmited by this define. The value of this define can be in the
 * range 2..(::POSCFG_TASKS_PER_PRIO * ::POSCFG_MAX_PRIO_LEVEL). This define
 * should be set as small as possible to decrease memory usage. Remember
 * that the OS has a built in idle task that also needs a task structure.
 * @note If ::POSCFG_DYNAMIC_MEMORY and ::POSCFG_DYNAMIC_REFILL are both
 * set to 1, ::POSCFG_MAX_TASKS can be set to 0 since the system will
 * dynamically allocate memory for additional task structures if the volume
 * of tasks defined by ::POSCFG_MAX_TASKS is exhausted.
 */
#define POSCFG_MAX_TASKS        16

/** Maximum count of events.
 * This define sets the maximum count of event data structures which can be
 * allocated. Semaphores, Mutexes, Flags and Message Boxes are using this
 * event data structures. Keep the value of this define as small as possible
 * to decrease memory usage. Note that there is no maximum value this define
 * can have, the pico]OS supports an unlimmit count of events.
 * @note If ::POSCFG_DYNAMIC_MEMORY and ::POSCFG_DYNAMIC_REFILL are both
 * set to 1, ::POSCFG_MAX_EVENTS can be set to 0 since the system will
 * dynamically allocate memory for additional events if the volume of events
 * defined by ::POSCFG_MAX_EVENTS is exhausted.
 */
#define POSCFG_MAX_EVENTS       16

/** Maximum count of message buffers.
 * This definition sets the maximum count of message buffers that can be
 * allocated with ::posMessageAlloc. Note that if only pointers are posted
 * with ::posMessageSend (::POSCFG_MSG_MEMORY == 0), this define sets the
 * count of internal buffers (=messages) that can be pending in the messaging
 * system. To get maximum performance, the value should be set to twice the
 * count of tasks that are sending messages.
 * If ::POSCFG_FEATURE_MSGBOXES is set to 0, this define has no effect.
 * @note If ::POSCFG_DYNAMIC_MEMORY and ::POSCFG_DYNAMIC_REFILL are both
 * set to 1, ::POSCFG_MAX_MESSAGES can be set to 0 since the system will
 * dynamically allocate additional message buffers if the volume of buffers
 * defined by ::POSCFG_MAX_MESSAGES is exhausted.
 */
#define POSCFG_MAX_MESSAGES      8

/** Maximum count of timers.
 * This define sets the maximum count of timers that can be allocated
 * with ::posTimerCreate. If ::POSCFG_FEATURE_TIMER is set to 0, this
 * define has no effect. Note that the value for this define
 * must be at least 1 if timers are enabled.
 * @note If ::POSCFG_DYNAMIC_MEMORY and ::POSCFG_DYNAMIC_REFILL are both
 * set to 1, ::POSCFG_MAX_TIMER can be set to 0 since the system will
 * dynamically allocate memory for additional timers if the volume of timers
 * defined by ::POSCFG_MAX_TIMER is exhausted.
 */
#define POSCFG_MAX_TIMER         4 

/** Set scheduling scheme.
 * The pico]OS supports two types of scheduling:<br>
 *
 * POSCFG_ROUNDROBIN = 0  sets the standard scheduling scheme, <br>
 * POSCFG_ROUNDROBIN = 1  enables the round robin scheduler.<br>
 *
 * For real time operation the standard scheduler is the best choice.
 * Standard scheduling means, that every task gets its own priority, at no
 * time two tasks can have the same priority. On a 8bit system you have a
 * maximum count of 8 x 8 = 64 tasks, with priority numbers 0 .. 63
 * where 0 is the  priority of the idle task and 63 is
 * the highest priority.<br>
 *
 * The round robin scheduler allows several tasks to have the same priority.
 * The number of tasks is limited by the count of bits the machine type
 * variable can hold (see define ::MVAR_BITS for details). The maximum count
 * of priority levels is also limited by ::MVAR_BITS. So on an 8bit system
 * you can have 8 priority levels with 8 tasks at each level, making 64
 * tasks in sum. The priority level 0 is the lowest, and the idle task runs
 * there. On priority 0 is space left for 7 user tasks. Priority 7 is the
 * highest priority. Note that on every priority level is done a round robin
 * scheduling, that means each task has the same right for execution.
 * No task is prefered. If a task gives of processing time, or its time slice
 * has expired, the next task at this priority is scheduled.
 */
#define POSCFG_ROUNDROBIN        1

/** Set soft multitasking.
 * By setting this define to 1, it is possible to soften the hard real time
 * scheduler. That means, a context switch is no more done every time an
 * event (such as semaphores or flags) is triggered, even if the signalled
 * task has a higher priority.
 * The signaled task will first start to run if the time slice of the
 * current task has expired or the task gives of processing time by itself
 * or the maximum count of signaled events is reached (see the definition
 * ::POSCFG_CTXSW_COMBINE for details, also the define
 * ::POSCFG_REALTIME_PRIO may interest you).
 * The purpose of soft multitasking is to reduce expensive context switches
 * to a minimum to get more processing time for the main work.
 */
#define POSCFG_SOFT_MTASK        1

/** Soft multitasking context switch combine counter trigger threshold.
 * Sets the number how often an event must be triggered to initiate a context
 * switch. This mechanism takes only effect when soft multitasking is enabled
 * (see definition ::POSCFG_SOFT_MTASK for details).
 * If the combine threshold is set to 0, a context switch will never
 * occure as result of triggering events.
 */
#define POSCFG_CTXSW_COMBINE    10

/** Realtime priority threshold for soft multitasking.
 * With this define some priority levels can be defined to be hard realtime,
 * even if soft multitasking is enabled. All priority levels equal to or
 * higher this value are scheduled in realtime.
 * If this value is set to 0, no priority levels are scheduled in realtime.
 * Note that this define takes only effect when ::POSCFG_SOFT_MTASK = 1
 * and ::POSCFG_ROUNDROBIN = 1.
 */
#define POSCFG_REALTIME_PRIO     0

/** When this define is set to a non-zero value, some user
 * available space is inserted into each task control block. The user
 * can call the function ::posTaskGetUserspace to get a pointer to the
 * user memory in the current tcb. Set this define to the number of bytes
 * you need to have in the task control block.
 */
#define POSCFG_TASKCB_USERSPACE  0

/** Enable the use of system supported message buffers.
 * By default, only buffer pointers can be posted with the messaging system.
 * If this definition is set to 1 (=enabled), the functions ::posMessageAlloc
 * and ::posMessageFree are supported to allow dynamic buffer allocation, and
 * real data with many bytes can be posted into the tasks message boxes.
 * Note: This requires that message boxes are enabled (the defintion
 * ::POSCFG_FEATURE_MSGBOXES must be set to 1).
 */
#define POSCFG_MSG_MEMORY        1

/** Size of message buffers in bytes.
 * If message boxes are enabled and ::POSCFG_MSG_MEMORY is set to 1,
 * this define sets the size of a message buffer. To keep the
 * operating system as simple as possible, only one fixed
 * buffer size is supported.
 */
#define POSCFG_MSG_BUFSIZE      80

/** Set number of software interrupts.
 * pico]OS has a built in mechanism to simulate software interrupts.
 * For example, software interrupts can be used to connect hardware
 * interrupts, that are outside the scope of pico]OS, to the realtime
 * operating system. A hardware interrupt will trigger a software
 * interrupt that can then signalize a semaphore object.
 * This define sets the number of software interrupts pico]OS will support.
 * @note  The define ::POSCFG_FEATURE_SOFTINTS must be set to 1 to have
 *        software interrupts compiled in.<br>
 *        If the nano-layer is linked to pico]OS, the lower four interrupts
 *        (numbers 0 through 3) are reserved for nano]OS. Make sure to set
 *        this define correctly (add 4) and do not use the lower 4
 *        interrupts in your application.
 */
#define POSCFG_SOFTINTERRUPTS    8

/** Set the number of software interrupts pico]OS shall be able to queue.
 * A software interrupt is executed each time the scheduler is called.
 * The scheduler runs at least with a rate of ::HZ times per second.
 * For example, if in your system 1000 software interrupts can happen
 * within a second, and the system tick rate is HZ = 100, set this define
 * at least to 10, else software interrupts could be lost. Since the
 * queue does not need much memory, it is saver to make the queue
 * longer than needed; I recommend twice the length calculated.
 * @note  The define ::POSCFG_FEATURE_SOFTINTS must be set to 1 to have
 *        software interrupts compiled in.
 */
#define POSCFG_SOFTINTQUEUELEN  20

/** Timer tick rate.
 * This define must be set to the tickrate of the timer
 * interrupt (= timer ticks per second).
 */
#define HZ                      18  /* timer ticks per second */

/** @} */



/*---------------------------------------------------------------------------
 *  PERFORMANCE / CODE SIZE / DEBUG SETTINGS
 *-------------------------------------------------------------------------*/

/** @defgroup codestyle Code Style
 * @ingroup configp
 * @{
 */

/** Configure code speed.
 * Set this define to 1 to get a fast code. Very much code lines are then
 * inlined instead of doing subroutine calls. Note that the fastest possible
 * code is generated when also the define ::POSCFG_SMALLCODE is set to 0.
 */
#define POSCFG_FASTCODE          1 

/** Configure code size.
 * Set this define to 1 to get a small code. This only touches some pico]OS
 * features, so expect not too much. Note that the smallest possible
 * code is generated when also the define ::POSCFG_FASTCODE is set to 0.
 */
#define POSCFG_SMALLCODE         1

/** Function argument checking.
 * There are three methods of argument checking:<br>
 *
 * <b>Type 0 </b><br>
 * Don't do any argument checking. This speeds up the code but
 * corrupted parameters would not be detected and could crash the
 * system. This is only recommandet for well tested embedded systems.<br>
 *
 * <b>Type 1 </b><br>
 * Do a minimum set of argument checking. For example, NULL pointers
 * will be detected. This is recommanded for a release version
 * of the operating system.<br>
 *
 * <b>Type 2 </b><br>
 * Do the full set of argument checking, including magic number tests
 * in internal data structures. This is the recommanded setting for
 * a debug version of the operating system.<br>
 */
#ifdef _DBG
#define POSCFG_ARGCHECK          2
#else
#define POSCFG_ARGCHECK          1
#endif

/** @} */



/*---------------------------------------------------------------------------
 *  DEFINE FEATURES TO INCLUDE
 *-------------------------------------------------------------------------*/

/** @defgroup feature Features
 * @ingroup configp
 * @{
 */

/** Include function ::posTaskYield.
 * If this definition is set to 1, the function ::posTaskYield will
 * be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_YIELD         1

/** Include function ::posTaskSleep.
 * If this definition is set to 1, the function ::posTaskSleep will
 * be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_SLEEP         1

/** Include function ::posTaskExit.
 * If this definition is set to 1, the function ::posTaskExit will
 * be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_EXIT          1

/** Include function ::posTaskGetPriority.
 * If this definition is set to 1, the function ::posTaskGetPriority will
 * be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_GETPRIORITY   1

/** Include function ::posTaskSetPriority.
 * If this definition is set to 1, the function ::posTaskSetPriority will
 * be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_SETPRIORITY   1

/** Include semaphore functions.
 * If this definition is set to 1, the semaphore functions are
 * added to the user API.
 */
#define POSCFG_FEATURE_SEMAPHORES    1

/** Include function ::posSemaDestroy.
 * If this definition is set to 1, the function ::posSemaDestroy will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_SEMAPHORES must be set to 1.
 */
#define POSCFG_FEATURE_SEMADESTROY   1

/** Include function ::posSemaWait.
 * If this definition is set to 1, the function ::posSemaWait will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_SEMAPHORES must be set to 1.
 */
#define POSCFG_FEATURE_SEMAWAIT      1

/** Include mutex functions.
 * If this definition is set to 1, the mutex functions are
 * added to the user API.
 */
#define POSCFG_FEATURE_MUTEXES       1

/** Include function ::posMutexDestroy.
 * If this definition is set to 1, the function ::posMutexDestroy will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_MUTEXES must be set to 1.
 */
#define POSCFG_FEATURE_MUTEXDESTROY  1

/** Include function ::posMutexTryLock.
 * If this definition is set to 1, the function ::posMutexTryLock will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_MUTEXES must be set to 1.
 */
#define POSCFG_FEATURE_MUTEXTRYLOCK  1

/** Include function ::posTaskGetCurrent.
 * If this definition is set to 1, the function ::posTaskGetCurrent will
 * be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_GETTASK       1

/** Include function ::posTaskUnused.
 * If this definition is set to 1, the function ::posTaskUnused will
 * be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_TASKUNUSED    1

/** Include message box functions.
 * If this definition is set to 1, the message box functions are
 * added to the user API.
 */
#define POSCFG_FEATURE_MSGBOXES      1

/** Include function ::posMessageWait.
 * If this definition is set to 1, the function ::posMessageWait will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_MSGBOXES must be set to 1.
 */
#define POSCFG_FEATURE_MSGWAIT       1

/** Include functions ::posTaskSchedLock and ::posTaskSchedUnlock.
 * If this definition is set to 1, the functions ::posTaskSchedLock
 * and ::posTaskSchedUnlock will be included into the pico]OS kernel.
 */
#define POSCFG_FEATURE_INHIBITSCHED  1

/** Include ::jiffies timer variable.
 * If this definition is set to 1, the ::jiffies timer variable
 * will be available.
 */
#define POSCFG_FEATURE_JIFFIES       1

/** Include timer functions.
 * If this definition is set to 1, the timer functions are
 * added to the user API.
 */
#define POSCFG_FEATURE_TIMER         1

/** Include function ::posTimerDestroy.
 * If this definition is set to 1, the function ::posTimerDestroy will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_TIMER must be set to 1.
 */
#define POSCFG_FEATURE_TIMERDESTROY  1

/** Include function ::posTimerFired.
 * If this definition is set to 1, the function ::posTimerFired will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_TIMER must be set to 1.
 */
#define POSCFG_FEATURE_TIMERFIRED    1

/** Include flags functions.
 * If this definition is set to 1, the flags functions are
 * added to the user API.
 */
#define POSCFG_FEATURE_FLAGS         1

/** Include function ::posFlagDestroy.
 * If this definition is set to 1, the function ::posFlagDestroy will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_FLAGS must be set to 1.
 */
#define POSCFG_FEATURE_FLAGDESTROY   1

/** Include function ::posFlagWait.
 * If this definition is set to 1, the function ::posFlagWait will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_FLAGS must be set to 1.
 */
#define POSCFG_FEATURE_FLAGWAIT      1

/** Include software interrupt functions.
 * If this definition is set to 1, the software interrupt functions are
 * added to the user API.
 */
#define POSCFG_FEATURE_SOFTINTS      1

/** Include function ::posSoftIntDelHandler.
 * If this definition is set to 1, the function ::posSoftIntDelHandler
 * will be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_SOFTINTS must be set to 1.
 */
#define POSCFG_FEATURE_SOFTINTDEL    1

/** Include function ::posInstallIdleTaskHook.
 * If this definition is set to 1, the function ::posInstallIdleTaskHook
 * will be added to the user API.
 */
#define POSCFG_FEATURE_IDLETASKHOOK  1

/** Enable atomic variable support.
 * If this definition is set to 1, the functions needed for accessing
 * atomic variables will be added to the user API.
 */
#define POSCFG_FEATURE_ATOMICVAR     1

/** Provide a task global error state variable.
 * If this definition is set to 1, the ::errno variable is supported.
 */
#define POSCFG_FEATURE_ERRNO         1

/** Enable list support.
 * If this definition is set to 1, the list functions are
 * added to the user API.
 */
#define POSCFG_FEATURE_LISTS         1

/** Include function ::posListJoin.
 * If this definition is set to 1, the function ::posListJoin will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_LISTS must be set to 1.
 */
#define POSCFG_FEATURE_LISTJOIN      1

/** Include function ::posListLen.
 * If this definition is set to 1, the function ::posListLen will
 * be included into the pico]OS kernel. Note that also
 * ::POSCFG_FEATURE_LISTS must be set to 1.
 */
#define POSCFG_FEATURE_LISTLEN       1

/** Enable the debug help.
 * If this definition is set to 1, pico]OS exports the global
 * variables ::picodeb_tasklist and ::picodeb_eventlist that
 * may help you debugging your code using an in-circuit debugger
 * with appropriated IDE.
 */
#define POSCFG_FEATURE_DEBUGHELP     0

/** @} */


#endif /* _POSCFG_H */
