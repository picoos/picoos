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
 * @file    port.h
 * @brief   port configuration file for the x86 Windows 32bit port
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: port.h,v 1.7 2006/03/14 18:44:26 dkuschel Exp $
 */


#ifndef _PORT_H
#define _PORT_H


/*---------------------------------------------------------------------------
 *  ARCHITECTURE / CPU SPECIFIC SETTINGS
 *-------------------------------------------------------------------------*/

/** @defgroup arch Architecture / CPU Specific Settings
 * @ingroup configp
 * @{
 */

/** Machine variable type.
 * This define is set to the variable type that best 
 * matches the target architecture. For example, define this
 * to @e char if the architecture has 8 bit, or to
 * @e int / @e long for a 32 bit architecture.
 * Note that the variable must fit into a single
 * memory cell of the target architecture.
 * (For a 32 bit architecture you can define MVAR_t to
 * @e char, @e short or @e int / @e long, whereas
 * at a 8 bit architecure you can only define it to @e char).
 * This define is responsible for the maximum count of tasks
 * the operating system can manage. For example:
 * @e char = 8 bit, 8 * 8 = 64 tasks;
 * @e long = 32 bit, 32 * 32 = 1024 tasks.
 */
#define MVAR_t                   int

/** Machine variable width.
 * This define tells the Operating System how much
 * bits can be stored in the machine variable type ::MVAR_t.
 * Some compilers support the sizeof(MVAR_t)-macro at this
 * position, but some others don't. For example, set
 * this define to 8 (bits) if ::MVAR_t is defined to @e char.
 */
#define MVAR_BITS                32  /* = (sizeof(MVAR_t) * 8) */

/** Integer variable type used for memory pointers.
 * This define must be set to an integer type that has the
 * same bit width like a memory pointer (e.g. void*) on
 * the target architecture. On a 32bit architecture you
 * would usually define this to @e long, for a 8 bit
 * architecture @e short would be sufficient.
 */
#define MPTR_t                   long

/** Required memory alignment on the target CPU.
 * To reach maximum speed, some architecures need correctly
 * aligned memory patterns. Set this define to the memory
 * alignment (in bytes) your architecture requires.
 * Note that this value must be a power of 2.
 * If your architecture does not require memory alignment,
 * set this value to 0 or 1.
 */
#define POSCFG_ALIGNMENT         4

/** Interruptable interrupt service routines.
 * This define must be set to 1 (=enabled) when an
 * interrupt service routine is interruptable on the machine.
 * E.g. some PowerPCs support critical interrupts that can
 * interrupt currently running noncritical interrupts.
 * If your machine configuration does not support interruptable ISRs,
 * you can set this define to 0 to save some execution time in ISRs.
 */
#define POSCFG_ISR_INTERRUPTABLE 0

/** Set the mechanism of stack memory handling.
 * There are three types of stack memory handling defined.<br>
 *
 * <b>Type 0 </b><br>
 * The stack memory is allocated by the user and a pointer
 * to the stack memory is passed to the functions
 * ::posTaskCreate, ::posInit and ::p_pos_initTask.<br>
 * 
 * <b>Type 1 </b><br>
 * The stack memory will be allocated by the platform port when a
 * new task is created. The memory will be freed when the task is
 * destroyed. The functions ::posTaskCreate, ::posInit and ::p_pos_initTask
 * are called with a parameter that specifies the stack size.
 * The function ::p_pos_freeStack is used to free the stack memory
 * again when the task is destroyed.<br>
 * 
 * <b>Type 2 </b><br>
 * Like type 1, but the size of the stack is fixed.
 * The functions ::posTaskCreate, ::posInit and ::p_pos_initTask do
 * not take any stack parameters.<br>
 * 
 * @note the functions ::posTaskCreate, ::posInit and ::p_pos_initTask
 * have different prototypes for each stack handling type.
 */
#define POSCFG_TASKSTACKTYPE     1

/** Enable call to function ::p_pos_initArch.
 * When this define is set to 1, the operating system will call
 * the user supplied function ::p_pos_initArch to initialize
 * the architecture specific portion of the operating system.
 */
#define POSCFG_CALLINITARCH      1

/** Enable dynamic memory.
 * If this define is set to 1, the memory for internal data structures
 * is allocated dynamically at startup. The define ::POS_MEM_ALLOC must
 * be set to a memory allocation function that shall be used. Otherwise,
 * when this define is set to 0, the memory is allocated statically.
 */
#define POSCFG_DYNAMIC_MEMORY    0

/** Dynamic memory management.
 * If this define is set to 1, the system will refill its volume of
 * system structures for tasks, events, timers and messages when the 
 * user requests more structures than the amount that was preallocated
 * (see defines ::POSCFG_MAX_TASKS, ::POSCFG_MAX_EVENTS,
 * ::POSCFG_MAX_MESSAGES and ::POSCFG_MAX_TIMER ).  To be able to use
 * this feature, you must also set the define ::POSCFG_DYNAMIC_MEMORY to 1.
 * But attention: The define ::POS_MEM_ALLOC must be set to a memory
 * allocation function <b>that is thread save</b>. Please set the define
 * ::POS_MEM_ALLOC to ::nosMemAlloc to use the nano layer memory allocator.
 */
#define POSCFG_DYNAMIC_REFILL    0

/** Define optional memory allocation function.
 * If ::POSCFG_DYNAMIC_MEMORY is set to 1, this definition must be set
 * to a memory allocation function such as "malloc". The memory allocation
 * function may not be reentrant when ::POSCFG_DYNAMIC_REFILL is set to 0,
 * since the multitasking system is not yet started when the function is
 * called.
 */
#define POS_MEM_ALLOC(bytes)     nosMemAlloc(bytes)

/** @} */



/*---------------------------------------------------------------------------
 *  LOCKING (DISABLE INTERRUPTS IN CRITICAL SECTIONS) 
 *-------------------------------------------------------------------------*/

/** @defgroup lock Disable / Enable Interrupts
 * @ingroup configp
 * The operating system must be able to disable the interrupts on the
 * processor for a short time to get exclusive access to internal data
 * structures. There are three possible ways to solve this:<br>
 *
 * 1) Most processors have assembler commands that directly allow
 * disabling and enabling interrupts. When the operating system needs
 * to get exclusive access to any data, it will disable interrupts,
 * access the data and enable interrupts again. The disadvantage of
 * this simple way is that if the processor had disabled interrupts
 * before the OS entered the critical section, the OS will reenable the
 * interrupts again after it left the critical section regardless if
 * interrupts were disabled before.<br>
 *
 * 2) A better way is to save the current processor state before disabling
 * interrupts. Much processors support a "push flags to stack" OP-Code
 * for this purpose. When the operating system enters a critical section,
 * it will push the processor flags to the stack and disables the interrupts.
 * Then, when the operating system will left the critical section again,
 * it simply restores the old processor state by popping the last
 * processor state from the stack. If interrupts where enabled before,
 * it just became enabled now.<br>
 *
 * 3) There are some processors which have no OP-code for directly pushing
 * the processor flags (=PSW, Processor Status Word) directly to the stack.
 * For this processors, you can define a local variable which will hold
 * the original PSW when the operating system enters the critical section.
 * If your processor has enough general purpose register, you may define
 * the variable as register variable for fastest possible access. This is
 * truly better than pushing the flags to the stack.
 * @{
 */

/** Enable local flags variable.
 * When this define is set to 1, a user defined variable will be
 * generated for storing the current processor state before
 * disabling interrupts. Then the define ::POSCFG_LOCK_FLAGSTYPE
 * must be set to the type of variable to be used for the flags.
 */
#define POSCFG_LOCK_USEFLAGS     1

/** Define variable type for the processor flags.
 * If ::POSCFG_LOCK_USEFLAGS is set to 1, this define must be
 * set to the variable type that shall be used for the
 * processor flags. In this example, the variable definition
 * "register VAR_t flags;" would be added to each function
 * using the macros ::POS_SCHED_LOCK and ::POS_SCHED_UNLOCK.
 */
#define POSCFG_LOCK_FLAGSTYPE    int

/** Scheduler locking.
 * Locking the scheduler for a short time is done by
 * disabling the interrupts on the processor. This macro
 * can contain a subroutine call or a piece of assembler
 * code that stores the processor state and disables
 * the interrupts. See ::POSCFG_LOCK_FLAGSTYPE for more details.
 */
#ifndef _X86ARCH_C
extern void p_pos_globalLock(int *flags);
#endif
#define POS_SCHED_LOCK           p_pos_globalLock(&flags)

/** Scheduler unlocking.
 * This is the counterpart macro of ::POS_SCHED_LOCK. It restores
 * the saved processor flags and reenables the interrupts this way.
 */
#ifndef _X86ARCH_C
extern void p_pos_globalUnlock(int flags);
#endif
#define POS_SCHED_UNLOCK         p_pos_globalUnlock(flags)

/** @} */



/*---------------------------------------------------------------------------
 *  FINDBIT - DEFINITIONS FOR GENERIC FILE fbit_gen.c
 *-------------------------------------------------------------------------*/

/** @defgroup findbit Generic Findbit
 * @ingroup configp
 * The pico]OS is shipped with a generic file that implements variouse
 * methods for finding the first and least significant bit set.
 * This section contains switches for configuring the file fbit_gen.c.
 * Please see the section <b>pico]OS Porting Information</b> for details
 * about findbit.
 * @{
 */

/** Generic finbit configuration, look-up table support.
 * The findbit mechanism can be implemented as look-up table.<br>
 *
 * POSCFG_FBIT_USE_LUTABLE = 0:<br>
 *  Do not use look up tables. "findbit" is implemented as a function.
 *  (This does not increase code size through tables. Also
 *  some CPUs may execute program code faster from their caches
 *  than fetching data from big lookup tables.)
 *  Note: This is the only possible setting for
 *        systems with ::MVAR_BITS != 8 <br>
 *
 * POSCFG_FBIT_USE_LUTABLE = 1:<br>
 *  - When round robin scheduling is disabled, findbit is done
 *    by use of a 256 byte sized lookup table.
 *  - When round robin scheduling is enabled, findbit is implemented
 *    as a function and uses a 256 byte sized lookup table.<br>
 *
 * POSCFG_FBIT_USE_LUTABLE = 2:<br>
 *  This is only applicable for round robin scheduling.
 *  "findbit" is implemented as a two dimensional lookup table.
 *  This blows code size very much.
 */
#define POSCFG_FBIT_USE_LUTABLE      0

/** Generic finbit configuration, machine bit-shift ability.
 * Some machines are very slow in doing bit-shifts. If your
 * target is such a machine, you can define this parameter to
 * zero to prevent findbit of doing excessive bitshifts.
 */
#define POSCFG_FBIT_BITSHIFT         1

/** @} */



/*---------------------------------------------------------------------------
 *  PORT DEPENDENT NANO LAYER CONFIGURATION
 *-------------------------------------------------------------------------*/

/** @defgroup portnlcfg Nano Layer Port
 * @ingroup configp
 * This section is used to configure port dependent
 * settings for the nano layer. (file port.h)
 * @{
 */

/** Set the direction the stack grows.
 * When the processor stack grows from bottom to top, this define
 * must be set to 1. On platforms where the stack grows from
 * top to bottom, this define must be set to 0.
 */
#define NOSCFG_STACK_GROWS_UP        0

/** Set the default stack size.
 * If the functions ::nosTaskCreate or ::nosInit are called with
 * a stack size of zero, this value is taken as the default stack size.
 */
#define NOSCFG_DEFAULT_STACKSIZE     0  /* no stack needed for W32 */

/** Enable generic console output handshake.
 * Please see description of function ::c_nos_putcharReady for details.
 */
#define NOSCFG_CONOUT_HANDSHAKE      0

/** Set the size of the console output FIFO.
 * If ::NOSCFG_CONOUT_HANDSHAKE is enabled, a FIFO buffer can be used
 * to speed up console output and to reduce CPU usage. This option is
 * useful when console output is done through a serial line that does
 * not have a hardware FIFO. To enable the FIFO, set this define to
 * the FIFO size in bytes. A zero will disable the FIFO buffer.
 */
#define NOSCFG_CONOUT_FIFOSIZE       256

/** @} */



/*---------------------------------------------------------------------------
 *  USER DEFINED CONTENT OF TASK ENVIRONMENT
 *-------------------------------------------------------------------------*/

#if DOX!=0
/** @def POS_USERTASKDATA
 * Add user defined data elements to the global task structure.
 * Please see detailed description of ::POSTASK_t.
 * @sa POSTASK_t
 */
#define POS_USERTASKDATA  void *stackptr;
#else


#define TASK_RESERVED_PORT_MEM  64

#define POS_USERTASKDATA \
   char portmem[TASK_RESERVED_PORT_MEM];


#endif /* DOX */


/*---------------------------------------------------------------------------
 *  SOME SPECIAL FUNCTIONS
 *-------------------------------------------------------------------------*/

#ifndef _X86ARCH_C

/* we support assertions */
#define HAVE_PLATFORM_ASSERT
extern void p_pos_assert(const char* text, const char *file, int line);

/* This function allows any MS Windows thread to call a pico]OS hardware
 * interrupt handler function. Thus, it maps a usual MS Windows thread
 * to a hardware interrupt from the view of pico]OS.
 * Argument:  Pointer to the interrupt handler to execute.
 * Note: This function must only be called by pure Windows threads,
 *       it is not allowed to call this function from a pico]OS task.
 */
extern void callInterruptHandler( void (*handlerfunc)(void) );

/* Call this function to enter a critical section of code. This function
 * works like the MS Windows function EnterCriticalSection(), but it is
 * allowed to call this function from anywhere you want, whereas the
 * MS Windows function can not be used in pico]OS tasks.
 * A critical section must be left again by calling singleThreadExit().
 */
extern void singleThreadEnter(void);

/* This is the counterpart function of singleThreadEnter().
 */
extern void singleThreadExit(void);

/* Idle task hook function ("suspend" the CPU for low power mode)
 */
extern void p_pos_idleTaskHook(void);
#define HOOK_IDLETASK   p_pos_idleTaskHook();

#endif


#endif /* _PORT_H */
