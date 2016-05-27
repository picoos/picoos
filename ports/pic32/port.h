/*
 * Copyright (c) 2014-2015, Ari Suutari <ari@stonepile.fi>.
 * Copyright (c) 2004,      Dennis Kuschel.
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


/**
 * @file    port.h
 * @brief   PPC440 port configuration file
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 */


#ifndef _PORT_H
#define _PORT_H

#include <xc.h>
#include <sys/attribs.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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
#define MVAR_t                int

/** Machine variable width.
 * This define tells the Operating System how much
 * bits can be stored in the machine variable type ::MVAR_t.
 * Some compilers support the sizeof(MVAR_t)-macro at this
 * position, but some others don't. For example, set
 * this define to 8 (bits) if ::MVAR_t is defined to @e char.
 */
#define MVAR_BITS              32  /* = (sizeof(MVAR_t) * 8) */

/** Integer variable type used for memory pointers.
 * This define must be set to an integer type that has the
 * same bit width like a memory pointer (e.g. void*) on
 * the target architecture. On a 32bit architecture you
 * would usually define this to @e long, for a 8 bit
 * architecture @e short would be sufficient.
 */
#define MPTR_t                int

/** Required memory alignment on the target CPU.
 * To reach maximum speed, some architecures need correctly
 * aligned memory patterns. Set this define to the memory
 * alignment (in bytes) your architecture requires.
 * Note that this value must be a power of 2.
 * If your architecture does not require memory alignment,
 * set this value to 0 or 1.
 */
#define POSCFG_ALIGNMENT        4

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
#ifdef POSNANO
#define POSCFG_TASKSTACKTYPE     1
#else
#define POSCFG_TASKSTACKTYPE     2
#endif

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
#define POSCFG_LOCK_USEFLAGS     1

/** Define variable type for the processor flags.
 * If ::POSCFG_LOCK_USEFLAGS is set to 1, this define must be
 * set to the variable type that shall be used for the
 * processor flags. In this example, the variable definition
 * "register VAR_t flags;" would be added to each function
 * using the macros ::POS_SCHED_LOCK and ::POS_SCHED_UNLOCK.
 */
#define POSCFG_LOCK_FLAGSTYPE    uint32_t

/** Scheduler locking.
 * Locking the scheduler for a short time is done by
 * disabling the interrupts on the processor. This macro
 * can contain a subroutine call or a piece of assembler
 * code that stores the processor state and disables
 * the interrupts. See ::POSCFG_LOCK_FLAGSTYPE for more details.
 */

#define POS_SCHED_LOCK          { flags = portSchedLock(); }
#define POS_IRQ_DISABLE_ALL     { flags = portIRQDisableAll(); }

/** Scheduler unlocking.
 * This is the counterpart macro of ::POS_SCHED_LOCK. It restores
 * the saved processor flags and reenables the interrupts this way.
 */

#define POS_SCHED_UNLOCK        { portSchedUnlock(flags); }
#define POS_IRQ_ENABLE_ALL      { portIRQEnableAll(flags); }

/** @} */



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
#define NOSCFG_STACK_GROWS_UP    0

/** Set the default stack size.
 * If the functions ::nosTaskCreate or ::nosInit are called with
 * a stack size of zero, this value is taken as the default stack size.
 */
#define NOSCFG_DEFAULT_STACKSIZE 64

/** Enable generic console output handshake.
 * Please see description of function ::c_nos_putcharReady for details.
 */
#define NOSCFG_CONOUT_HANDSHAKE      1

/** Set the size of the console output FIFO.
 * If ::NOSCFG_CONOUT_HANDSHAKE is enabled, a FIFO buffer can be used
 * to speed up console output and to reduce CPU usage. This option is
 * useful when console output is done through a serial line that does
 * not have a hardware FIFO. To enable the FIFO, set this define to
 * the FIFO size in bytes. A zero will disable the FIFO buffer.
 */
#define NOSCFG_CONOUT_FIFOSIZE       80

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
#define POS_USERTASKDATA  struct PortStack *stackptr;
#else

#if (POSCFG_TASKSTACKTYPE == 1)

#define POS_USERTASKDATA \
    struct PortStack  *stackptr;          \
    unsigned char     *stack;             \
    UINT_t            stackSize;
#elif (POSCFG_TASKSTACKTYPE == 2)

#define POS_USERTASKDATA \
   struct PortStack  *stackptr; \
   unsigned char stack[PORTCFG_FIXED_STACK_SIZE];

#endif

#endif /* DOX */


/*---------------------------------------------------------------------------
 *  ADDITIONAL DEFINES FOR THIS PORT
 *-------------------------------------------------------------------------*/

/**
 * Include support for delayed context switch after interrupt
 * (done using software interrupt #0).
 */
#define POSCFG_INT_EXIT_QUICK 1

/* provide NULL pointer */
#ifndef NULL
#define NULL ((void*)0)
#endif

/**
 * To detect stack overflows, fill stack area with
 * PORT_STACK_MAGIC during initialization.
 */
#define PORT_STACK_MAGIC       0x56

/**
 * MIPS likes that stack is 8 byte aligned,
 * this is related to pushing doublewords,
 * I think.
 */
#define PORT_STACK_ALIGNMENT   8

/**
 * MIPS calling convention states that
 * caller must provide slots for 4 arguments
 * in stack, even they are not used by caller.
 */
#define PORT_STACK_ARGSLOTS    (4*4)

/**
 * Task stack frame for PIC32 (mips) CPU.
 */
struct PortStack {

  uint32_t cp0Status;
  uint32_t cp0Epc;
  uint32_t cp0Cause;

  union {

    uint32_t r[28];
    struct {

      uint32_t at;

      uint32_t v0;
      uint32_t v1;

      uint32_t a0;
      uint32_t a1;
      uint32_t a2;
      uint32_t a3;

      uint32_t t0;
      uint32_t t1;
      uint32_t t2;
      uint32_t t3;
      uint32_t t4;
      uint32_t t5;
      uint32_t t6;
      uint32_t t7;

      uint32_t s0;
      uint32_t s1;
      uint32_t s2;
      uint32_t s3;
      uint32_t s4;
      uint32_t s5;
      uint32_t s6;
      uint32_t s7;

      uint32_t t8;
      uint32_t t9;

      uint32_t gp;
      uint32_t fp;
      uint32_t ra;
    };
  };

  uint32_t mflo;
  uint32_t mfhi;

};


#ifdef _DBG
#define HAVE_PLATFORM_ASSERT
extern void p_pos_assert(const char* text, const char *file, int line);
#endif

/**
 * Macros to save context of current stack.
 */

#define PORT_PUT_REG2(sp, r,f)                       \
  asm volatile ("sw $" #r ", %[p1]($" #sp ")"        \
    : : [p1] "i" (offsetof(struct PortStack, f)))

#define PORT_GET_REG2(sp, r,f)                       \
  asm volatile ("lw $" #r ", %[p1]($" #sp ")"        \
    : : [p1] "i" (offsetof(struct PortStack, f)))

#define PORT_PUT_REG1(r) PORT_PUT_REG2(t0,r,r)
#define PORT_GET_REG1(r) PORT_GET_REG2(t0,r,r)

/**
 * Step1: Save CP0 registers, some work registers
 *  and set up interrupt stack.
 */
#define portSaveContext1() {                         \
  asm volatile(".set push                      \n\t" \
               ".set at");                           \
  asm volatile("addiu $sp, $sp, -%[framesize]"       \
    : : [framesize] "i" (sizeof(struct PortStack))); \
  asm volatile("mfc0   $k0, $14");                   \
  PORT_PUT_REG2(sp, k0, cp0Epc);                     \
  asm volatile("mfc0   $k0, $13");                   \
  PORT_PUT_REG2(sp, k0, cp0Cause);                   \
  asm volatile("mfc0   $k1, $12");                   \
  PORT_PUT_REG2(sp, k1, cp0Status);                  \
  asm volatile(".set noat");                         \
  PORT_PUT_REG2(sp, at, at);                         \
  asm volatile(".set pop");                          \
  PORT_PUT_REG2(sp, t0, t0);                         \
  PORT_PUT_REG2(sp, t1, t1);                         \
  PORT_PUT_REG2(sp, t2, t2);                         \
  asm volatile("move  $t0, $sp                 \n\t" \
               "lw    $t1, portIntNesting_g    \n\t" \
               "bne   $t1, $zero, 1f           \n\t" \
               "lw    $t2, posCurrentTask_g    \n\t" \
               "sw    $t0, %[stackptr]($t2)    \n\t" \
               "la    $sp, _stack              \n"   \
       "1:                                     \n\t" \
               "addiu $sp,$sp,-%[argslots]     \n\t" \
               "addiu $t1,$t1,1                \n\t" \
               "sw    $t1,portIntNesting_g     \n\t" \
 : : [argslots]"i" (PORT_STACK_ARGSLOTS),            \
     [stackptr]"i" (offsetof(struct POSTASK, stackptr))); \
}

/**
 * Step2: Enable nested interrupts.
 */
#define portSaveContext2() \
asm volatile("ins $k1, $zero, 1, 15  \n\t"  /* Set IPL, UM, ERL & EXL to 0 */   \
             "ext $k0, $k0, 10, 6  \n\t"          /* Get RIPL */ \
             "ins $k1, $k0, 10, 6   \n\t"          /* Copy RIPL -> IPL */ \
             "mtc0 $k1, $12")               /* Update status */

/**
 * Step3: Save rest of the registers, using t0 as stack pointer.
 */
#define portSaveContext3() {                         \
  PORT_PUT_REG1(v0);                                 \
  PORT_PUT_REG1(v1);                                 \
  PORT_PUT_REG1(a0);                                 \
  PORT_PUT_REG1(a1);                                 \
  PORT_PUT_REG1(a2);                                 \
  PORT_PUT_REG1(a3);                                 \
  PORT_PUT_REG1(t3);                                 \
  PORT_PUT_REG1(t4);                                 \
  PORT_PUT_REG1(t5);                                 \
  PORT_PUT_REG1(t6);                                 \
  PORT_PUT_REG1(t7);                                 \
  PORT_PUT_REG1(s0);                                 \
  PORT_PUT_REG1(s1);                                 \
  PORT_PUT_REG1(s2);                                 \
  PORT_PUT_REG1(s3);                                 \
  PORT_PUT_REG1(s4);                                 \
  PORT_PUT_REG1(s5);                                 \
  PORT_PUT_REG1(s6);                                 \
  PORT_PUT_REG1(s7);                                 \
  PORT_PUT_REG1(t8);                                 \
  PORT_PUT_REG1(t9);                                 \
  PORT_PUT_REG1(gp);                                 \
  PORT_PUT_REG1(fp);                                 \
  PORT_PUT_REG1(ra);                                 \
  asm volatile("mflo   $t1");                        \
  asm volatile("mfhi   $t2");                        \
  PORT_PUT_REG2(t0, t1, mflo);                       \
  PORT_PUT_REG2(t0, t2, mfhi);                       \
  if (POSCFG_ARGCHECK > 1)                           \
    if (portIntNesting_g == 1)                       \
      P_ASSERT("TStk", (posCurrentTask_g->stack[0] == PORT_STACK_MAGIC)); \
}

#define portSaveContext() {                          \
  portSaveContext1();                                \
  portSaveContext2();                                \
  portSaveContext3();                                \
}

/**
 * Restore context for current task.
 */
#define portRestoreContext()                  \
  asm volatile("j portRestoreContextImpl\n\t" \
               "nop")

extern void portRestoreContextImpl(void);
extern unsigned char *portIrqStack;
extern unsigned int _stack[];
extern int portIntNesting_g;

/**
 * Max IPL level used by Pico]OS. Interrupts having IPL
 * greater than this are not blocked by POS_SCHED_LOCK, so
 * they are always fast (but cannot call most pico]OS functions).
 */
#define PORT_MAX_IPL        5

/*
 * __optimize("omit-frame-ponter") is needed to omit frame pointer in
 * naked functions. This looks like a bug in gcc currently.
 */

#define PORT_NAKED __attribute__((naked)) \
                   __attribute__ ((__optimize__("omit-frame-pointer")))

void portIdleTaskHook(void);
#define HOOK_IDLETASK   portIdleTaskHook();

uint32_t portSchedLock(void);
void portSchedUnlock(uint32_t);
uint32_t portIRQDisableAll(void);
void portIRQEnableAll(uint32_t);

void portInitClock(void);
void portInitConsole(void);
bool portCanSleep(void);

#endif /* _PORT_H */
