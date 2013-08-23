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
 * @brief   AVR port configuration file
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: port.h,v 1.12 2008/08/30 09:33:28 smocz Exp $
 */


#ifndef _PORT_H
#define _PORT_H

#include <inttypes.h>

/*---------------------------------------------------------------------------
 *  ARCHITECTURE / CPU SPECIFIC SETTINGS
 *-------------------------------------------------------------------------*/

/** @defgroup arch Configuration: Architecture / CPU Specific Settings
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
#define MVAR_t               char

/** Machine variable width.
 * This define tells the Operating System how much
 * bits can be stored in the machine variable type ::MVAR_t.
 * Some compilers support the sizeof(MVAR_t)-macro at this
 * position, but some others don't. For example, set
 * this define to 8 (bits) if ::MVAR_t is defined to @e char.
 */
#define MVAR_BITS               8  /* = (sizeof(MVAR_t) * 8) */

/** Integer variable type used for memory pointers.
 * This define must be set to an integer type that has the
 * same bit width like a memory pointer (e.g. void*) on
 * the target architecture. On a 32bit architecture you
 * would usually define this to @e long, for a 8 bit
 * architecture @e short would be sufficient.
 */
#define MPTR_t               long

/** Required memory alignment on the target CPU.
 * To reach maximum speed, some architecures need correctly
 * aligned memory patterns. Set this define to the memory
 * alignment (in bytes) your architecture requires.
 * Note that this value must be a power of 2.
 * If your architecture does not require memory alignment,
 * set this value to 0 or 1.
 */
#define POSCFG_ALIGNMENT         0

/** Interruptable interrupt service routines.
 * This define must be set to 1 (=enabled) when an
 * interrupt service routine is interruptable on the machine.
 * E.g. some PowerPCs support critical interrupts that can
 * interrupt currently running noncritical interrupts.
 * If your machine configuration does not support interruptable ISRs,
 * you can set this define to 0 to save some execution time in ISRs.
 */
#define POSCFG_ISR_INTERRUPTABLE 1

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
#define POSCFG_TASKSTACKTYPE     2

/** Enable call to function ::p_pos_initArch.
 * When this define is set to 1, the operating system will call
 * the user supplied function ::p_pos_initArch to initialize
 * the architecture specific portion of the operating system.
 * TODO: why can this feature switch on/off?
 *  (answer:  because not all ports need an initarch-function)
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

/** @defgroup lock Configuration: Disable / Enable Interrupts
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
#define POSCFG_LOCK_USEFLAGS     0

/** Define variable type for the processor flags.
 * If ::POSCFG_LOCK_USEFLAGS is set to 1, this define must be
 * set to the variable type that shall be used for the
 * processor flags. In this example, the variable definition
 * "register VAR_t flags;" would be added to each function
 * using the macros ::POS_SCHED_LOCK and ::POS_SCHED_UNLOCK.
 */
#define POSCFG_LOCK_FLAGSTYPE    register VAR_t

/** Scheduler locking.
 * Locking the scheduler for a short time is done by
 * disabling the interrupts on the processor. This macro
 * can contain a subroutine call or a piece of assembler
 * code that stores the processor state and disables
 * the interrupts. See ::POSCFG_LOCK_FLAGSTYPE for more details.
 */
#define POS_SCHED_LOCK       __asm__ __volatile__( \
                                    "in __tmp_reg__, __SREG__"  "\n\t" \
                                    "cli"                       "\n\t" \
                                    "push __tmp_reg__"          "\n\t" \
                                    :: )

/** Scheduler unlocking.
 * This is the counterpart macro of ::POS_SCHED_LOCK. It restores
 * the saved processor flags and reenables the interrupts this way.
 */
#define POS_SCHED_UNLOCK     __asm__ __volatile__( \
                                    "pop __tmp_reg__"           "\n\t" \
                                    "out __SREG__, __tmp_reg__" "\n\t" \
                                    :: )

/** @} */


/*---------------------------------------------------------------------------
 *  INTERRUPT SERVICE ROUTINE FRAME
 *-------------------------------------------------------------------------*/

// Interrupt service routine stack size (in bytes)
#define ISR_STACK_SIZE     80

// The variable is declared in the file arch_c.c
extern uint8_t isrStackMem_g[];

/**
 * This function is implement in arch_c.c
 */
void interruptReturn(void) __attribute__ ((naked));


#if defined(RAMPZ)

#define SAVE_RAMPZ      "in     r0, 0x3b" "\n\t" \
                        "push   r0"     "\n\t"

#define RESTORE_RAMPZ   "pop    r0" "\n\t" \
                        "out    0x3b, r0"     "\n\t"
#else

#define SAVE_RAMPZ    ""
#define RESTORE_RAMPZ ""

#endif

#if defined(EIND)

#define SAVE_EIND       "in     r0, 0x3c" "\n\t" \
                        "push   r0"     "\n\t"

#define RESTORE_EIND    "pop    r0" "\n\t" \
                        "out    0x3c, r0"     "\n\t"

#else

#define SAVE_EIND    ""
#define RESTORE_EIND ""

#endif

/**
 * Macro for saving the context during an interrupt service.
 */
#define SAVE_CONTEXT(void) \
    __asm__ __volatile__ ( \
        "push   r0"     "\n\t" \
        "in     r0, __SREG__" "\n\t" \
        "cli"           "\n\t" \
		"push   r0"     "\n\t" \
        SAVE_RAMPZ \
		SAVE_EIND \
		"push   r1"     "\n\t" \
        "clr    __zero_reg__"   "\n\t" \
        "push   r2"     "\n\t" \
        "push   r3"     "\n\t" \
        "push   r4"     "\n\t" \
        "push   r5"     "\n\t" \
        "push   r6"     "\n\t" \
        "push   r7"     "\n\t" \
        "push   r8"     "\n\t" \
        "push   r9"     "\n\t" \
        "push   r10"    "\n\t" \
        "push   r11"    "\n\t" \
        "push   r12"    "\n\t" \
        "push   r13"    "\n\t" \
        "push   r14"    "\n\t" \
        "push   r15"    "\n\t" \
        "push   r16"    "\n\t" \
        "push   r17"    "\n\t" \
        "push   r18"    "\n\t" \
        "push   r19"    "\n\t" \
        "push   r20"    "\n\t" \
        "push   r21"    "\n\t" \
        "push   r22"    "\n\t" \
        "push   r23"    "\n\t" \
        "push   r24"    "\n\t" \
        "push   r25"    "\n\t" \
        "push   r26"    "\n\t" \
        "push   r27"    "\n\t" \
        "push   r28"    "\n\t" \
        "push   r29"    "\n\t" \
        "push   r30"    "\n\t" \
        "push   r31"    "\n\t" \
    ::  \
    )


/**
 * Macro for restoring the context during an interrupt service.
 */
#define RESTORE_CONTEXT(void) \
    __asm__ __volatile__ ( \
        "pop    r31"    "\n\t" \
        "pop    r30"    "\n\t" \
        "pop    r29"    "\n\t" \
        "pop    r28"    "\n\t" \
        "pop    r27"    "\n\t" \
        "pop    r26"    "\n\t" \
        "pop    r25"    "\n\t" \
        "pop    r24"    "\n\t" \
        "pop    r23"    "\n\t" \
        "pop    r22"    "\n\t" \
        "pop    r21"    "\n\t" \
        "pop    r20"    "\n\t" \
        "pop    r19"    "\n\t" \
        "pop    r18"    "\n\t" \
        "pop    r17"    "\n\t" \
        "pop    r16"    "\n\t" \
        "pop    r15"    "\n\t" \
        "pop    r14"    "\n\t" \
        "pop    r13"    "\n\t" \
        "pop    r12"    "\n\t" \
        "pop    r11"    "\n\t" \
        "pop    r10"    "\n\t" \
        "pop    r9"     "\n\t" \
        "pop    r8"     "\n\t" \
        "pop    r7"     "\n\t" \
        "pop    r6"     "\n\t" \
        "pop    r5"     "\n\t" \
        "pop    r4"     "\n\t" \
        "pop    r3"     "\n\t" \
        "pop    r2"     "\n\t" \
        "pop    r1"     "\n\t" \
		RESTORE_EIND \
        RESTORE_RAMPZ \
        "pop    r0"     "\n\t" \
        "out    __SREG__, r0" "\n\t" \
        "pop    r0"    "\n\t" \
    ::  \
    )

/**
 * This macro is used to define an interrupt service routine for using
 * API functions from picoos.
 * The macro generate the appropriated frame for that and delegates
 * to an handler.
 * See an example for handle a interrupt from the UART:
 *
 * @code
 *      static void handleUartRecive( void );
 *      static void handleUartRecive( void ) {
 *          posSemaSignal(semaReceive);
 *      }
 *
 *      PICOOS_SIGNAL( SIG_UART_RECV, handleUartRecive )
 *
 * @endcode
 *
 * @param signalname The name of the signal for generate this frame.
 *
 * @param handler The function to handle the interrupt.
 *
 */
#define PICOOS_SIGNAL(signame, handler)   \
void signame (void) __attribute__ ((signal, naked));        \
void signame (void) {                   \
    SAVE_CONTEXT();                     \
    if (posInInterrupt_g == 0) {        \
       posCurrentTask_g->stackptr = (void*)SP; \
       SP = (uint16_t)(isrStackMem_g + ISR_STACK_SIZE -1); \
    }                                   \
    c_pos_intEnter();                   \
    handler();                          \
    c_pos_intExit();                    \
    SP = (uint16_t)posCurrentTask_g->stackptr; \
    __asm__ __volatile__("jmp    interruptReturn"); \
}                                       \


/*---------------------------------------------------------------------------
 *  FINDBIT - DEFINITIONS FOR GENERIC FILE fbit_gen.c
 *-------------------------------------------------------------------------*/

/** @defgroup findbit Configuration: Generic Findbit
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

/** @defgroup portnlcfg Configuration: Nano Layer Port
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
#define NOSCFG_DEFAULT_STACKSIZE     64
// 128

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
#define NOSCFG_CONOUT_FIFOSIZE       20

/** @} */



/*---------------------------------------------------------------------------
 *  USER DEFINED CONTENT OF TASK ENVIRONMENT
 *-------------------------------------------------------------------------*/

#if (DOX!=0)
/** @def POS_USERTASKDATA
 * Add user defined data elements to the global task structure.
 * Please see detailed description of ::POSTASK_t.
 * @sa POSTASK_t
 */
#define POS_USERTASKDATA  void *stackptr;
#else

/* Here is an example for different memory handling types:
 */

#if (POSCFG_TASKSTACKTYPE == 0)

#define POS_USERTASKDATA \
   void  *stackptr;

#elif (POSCFG_TASKSTACKTYPE == 1)

#define POS_USERTASKDATA \
   void  *stackptr; \
   void  *stackroot;

#elif (POSCFG_TASKSTACKTYPE == 2)

/* context switch (stack frame):   35 bytes
 * os (call depth)  4*2        :    8 bytes
 * runtime lib + application   : 20 + 20 = 40 bytes
 *    sum = 128
 */

#define FIXED_STACK_SIZE 0x80
#define POS_USERTASKDATA \
   void  *stackptr; \
   unsigned char stack[FIXED_STACK_SIZE];

#endif

#endif /* DOX */

#endif /* _PORT_H */
