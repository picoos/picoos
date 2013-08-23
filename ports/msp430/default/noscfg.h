/*
 *  Copyright (c) 2004-2005, Dennis Kuschel.
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
 * @file    noscfg.h
 * @brief   pico]OS nano layer configuration file for the 6502 port
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: noscfg.h,v 1.5 2011/11/27 09:03:46 ari Exp $
 */

#ifndef _NOSCFG_H
#define _NOSCFG_H



/*---------------------------------------------------------------------------
 *  MEMORY MANAGEMENT
 *-------------------------------------------------------------------------*/

/** @defgroup cfgnosmem Dynamic Memory Management
 * @ingroup confign
 * @{
 */

/** Include dynamic memory management functions.
 * The nano layer supports an own implementation of the malloc() and
 * free() memory functions, and it has a multitasking able framework
 * for external memory functions. If this define is set to 1, the memory
 * management functions will be compiled into the nano layer.
 * @sa  NOSCFG_MEM_MANAGER_TYPE, NOSCFG_MEM_MANAGE_MODE,
 *      nosMemAlloc, nosMemFree
 */
#define NOSCFG_FEATURE_MEMALLOC      1

/** Set type of memory manager. Three types are possible: @n
 *   0 = Use the malloc/free functions from the runtime library @n
 *   1 = Use internal nano layer memory allocator. The system variables
 *       ::__heap_start and ::__heap_end must be provided and initialized
 *       by the user. See also define ::NOSCFG_MEM_MANAGER_TYPE. @n
 *   2 = The user supplys its own memory allocation routines.
 *       See defines ::NOSCFG_MEM_USER_MALLOC and ::NOSCFG_MEM_USER_FREE.
 */
#ifdef __arm__
#define NOSCFG_MEM_MANAGER_TYPE      1
#else
#define NOSCFG_MEM_MANAGER_TYPE      1
#endif

/** This is a pointer to the start of the memory heap.
 * It can either be a real variable pointing to heap memory,
 * or it can be a simple static constant define. It is recommended
 * to let the linker generate a label with this name that points
 * to the start of unused RAM memory.
 */
extern void *__heap_start;

/** This is a pointer to the end of the memory heap.
 * It can either be a real variable pointing to the end of heap memory,
 * or it can be a simple static constant define. It is recommended
 * to let the linker generate a label with this name that points
 * to the end of unused RAM memory.
 */
extern void *__heap_end;

/** User defined memory allocation function.
 * The user may supply its own memory allocator functions.
 * This define is set to the function name of the user's
 * memory allocation function.
 * @note ::NOSCFG_MEM_MANAGER_TYPE must be set to type 2
 *       to enable the use of a user supplied memory allocator.
 */
#define NOSCFG_MEM_USER_MALLOC       mymalloc

/** User defined memory free function.
 * The user may supply its own memory allocator functions.
 * This define is set to the function name of the user's
 * memory free function.
 * @note ::NOSCFG_MEM_MANAGER_TYPE must be set to type 2
 *       to enable the use of a user supplied memory allocator.
 */
#define NOSCFG_MEM_USER_FREE         myfree

/** Number of bytes reserved in the stack frame for the memory allocator.
 * If heap memory is used for stack memory at task creation, there can
 * be an ugly side effect: When the task terminates again, and the RTOS
 * frees the stack memory by calling a mem-free function, the memory
 * allocator will chain the memory block into a list of free blocks
 * (=> memory pointers will be written to the top or bottom of the
 * no more used stack frame). But the stack frame remains still active
 * for a couple of subroutine jump backs, so the memory block is still
 * modified even it is assumed to be unused. To avoid the corruption of
 * memory pointers in the stack frame block, this define can be used to
 * reserve some space for the memory allocator at the root of the stack
 * frame. If the processor stack grows from top to bottom (most usual),
 * the stack root is the highest memory location, and in most cases
 * this define can be set to zero (= don't reserve any space).
 */
#define NOSCFG_STKMEM_RESERVE        0

/** Set the working scheme of the internal nano layer memory allocator. @n
 *   0 = very simple manager, low code size, but heap can fragment. @n
 *   1 = low heap fragmentation, mem-free is slower than in mode 0,
 *       needs more code memory.
 * @note ::NOSCFG_MEM_MANAGER_TYPE must be set to type 1 to
 *       enable the internal memory allocator.
 */
#define NOSCFG_MEM_MANAGE_MODE       0

/** Include function ::nosMemSet.
 * If this definition is set to 1, the function ::nosMemSet will
 * be included into the nano layer.
 */
#define NOSCFG_FEATURE_MEMSET        0

/** Include function ::nosMemCopy.
 * If this definition is set to 1, the function ::nosMemCopy will
 * be included into the nano layer.
 */
#define NOSCFG_FEATURE_MEMCOPY       0
#define NOSCFG_MEM_OVWR_STANDARD     0

/** @} */



/** @defgroup cfgabstr Abstracted Functions
 * @ingroup confign
 * @{
 */

/*---------------------------------------------------------------------------
 *  TASK CREATION
 *-------------------------------------------------------------------------*/

/** Enable nano task create function.
 * The nano layer supports a highly portable version of the pico]OS's
 * ::posTaskCreate function. Even if the parameter list of the function
 * ::posTaskCreate can be different dependent on the platform port, the
 * nano task create function has a fixed parameter list. The second
 * advantage of the nano task create function is the fact that it is
 * able to dynamically allocate the stack frame, making the stack frame
 * just as big the application requires.<br>
 *
 * Set this define to 1 to enable the nano task create function
 * ::nosTaskCreate.
 *
 * @note   If this define is set to 1, also the feature
 *         ::NOSCFG_FEATURE_MEMALLOC must be enabled on platforms that
 *         run with ::POSCFG_TASKSTACKTYPE = 0.
 */
#define NOSCFG_FEATURE_TASKCREATE    0

/** @} */



/*---------------------------------------------------------------------------
 *  CONSOLE INPUT / OUTPUT
 *-------------------------------------------------------------------------*/

/** @defgroup cfgnoscio Console I/O
 * @ingroup confign
 * @{
 */

/** Enable generic console input support.
 * If this define is set to 1, the functions ::nosKeyGet and ::nosKeyPressed
 * are added to the user API of the nano layer.
 * @note  The platform port must support console input. The port can
 *        either call ::c_nos_keyinput or rise the software interrupt 0
 *        to feed keyboard data into the nano layer.
 */
#define NOSCFG_FEATURE_CONIN         1

/** Set keyboard buffer size (in bytes).
 * If the console input is enabled (when ::NOSCFG_FEATURE_CONIN is to 1),
 * this define sets the depth of the keyboard buffer that is implemented
 * in the nano layer.
 */
#define NOSCFG_CONIO_KEYBUFSIZE      10

/** Enable generic console output support.
 * If this define is set to 1, the functions ::nosPrintChar, ::nosPrint
 * and ::nosPrintf are added to the user API of the nano layer.
 * Note that if you wish to have the formatted print functions compiled
 * in, you must also set the define ::NOSCFG_FEATURE_PRINTF to 1.
 * @note  The platform port must support console output, it must
 *        export the function ::p_putchar.
 */
#define NOSCFG_FEATURE_CONOUT        1

/** Enable generic printf functions.
 * The nano layer supports a set of realy generic printf functions.
 * This functions are not variadic, that means they do not support
 * a variable parameter list, thus they can be compiled without having
 * a runtime library linked (the header file <stdarg.h> is not needed).
 * @sa    ::nosPrintf1, ::NOSCFG_FEATURE_SPRINTF
 */
#define NOSCFG_FEATURE_PRINTF        0

/** Enable generic string printf ('sprintf') functions.
 * The nano layer supports a set of realy generic sprintf functions.
 * This functions are not variadic, that means they do not support
 * a variable parameter list, thus they can be compiled without having
 * a runtime library linked (the header file <stdarg.h> is not needed).
 * @sa    ::nosSPrintf1, ::NOSCFG_FEATURE_PRINTF
 */
#define NOSCFG_FEATURE_SPRINTF       0

/** @} */



/*---------------------------------------------------------------------------
 *  BOTTOM HALFS
 *-------------------------------------------------------------------------*/

/** @defgroup cfgnosbh Bottom Halfs
 * @ingroup confign
 * @{
 */

/** Enable bottom half support.
 * If this definition is set to 1, the bottom half functions are
 * added to the user API.
 */
#define NOSCFG_FEATURE_BOTTOMHALF    0

/** Maximum count of bottom halfs.
 * This define sets the maximum count of bottom halfs the operating system
 * can handle. The count must be at least 1 and must not exceed ::MVAR_BITS.
 */
#define NOS_MAX_BOTTOMHALFS          8

/** @} */



/*---------------------------------------------------------------------------
 *  CPU USAGE
 *-------------------------------------------------------------------------*/

/** @defgroup cfgcpuu CPU Usage
 * @ingroup confign
 * @{
 */

/** Enable calculation of CPU usage.
 * If this definition is set to 1, the CPU usage is calculated from
 * within the idle task, and the function ::nosCpuUsage is added to
 * the user API.
 */
#define NOSCFG_FEATURE_CPUUSAGE      0

/** @} */



/*---------------------------------------------------------------------------
 *  KEY STRINGS
 *-------------------------------------------------------------------------*/

/** @defgroup cfgnoskey Key Strings
 * @ingroup confign
 * @{
 */

/** Enable key string support.
 * If this definition is set to 1, the key string functions are
 * added to the user API.
 */
#define NOSCFG_FEATURE_REGISTRY    0

/** Enable the query for key strings.
 * If this definition is set to 1, the functions ::nos_keyQueryBegin,
 * ::nos_keyQueryElem and ::nos_keyQueryEnd will be added to the user API.
 */
#define NOSCFG_FEATURE_REGQUERY      1

/** Enable user keys. User keys behave something like a 'registry'
 * for binary values. A binary value can be assigned to a text-string, and
 * the string is stored into the system keylist. An application can now
 * ask the system for a keystring and will get the belonging binary value.
 * If this definition is set to 1, the functions ::nos_keyGet,
 * ::nos_keySet and ::nos_keyDel will be added to the user API.
 */
#define NOSCFG_FEATURE_USERREG      1

/** Maximum length a key string can have.
 * This define sets the maximum length (characters) a key string can have.
 * If key strings are enabled, this define must be set to at least 4.
 */
#define NOS_MAX_REGKEYLEN               8

/** Key string housekeeping.
 * When ever a new key is created, the nano layer needs to call malloc().
 * If this define is set to a number greater than 1, one malloc-call is
 * used to allocate memory for a bunch of keys (the count of keys is
 * defined here). This reduces overhead in the memory manager, but
 * increases the code-memory usage a bit.
 */
#define NOS_REGKEY_PREALLOC             4

/** @} */

/**
 * Console peripheral type.
 * 0: default, usually USCI_A if available,
 * 1: USCI_A,
 * 2: UART 0
 */
#define PORTCFG_CON_PERIPH        0


/*---------------------------------------------------------------------------
 *  ADDITIONAL USER SETTINGS FOR THE MSP430 PORT
 *-------------------------------------------------------------------------*/

#endif /* _NOSCFG_H */
