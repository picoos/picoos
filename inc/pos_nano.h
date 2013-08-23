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
 * @file    pos_nano.h
 * @brief   pico]OS nano layer main include file
 * @author  Dennis Kuschel
 *
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: pos_nano.h,v 1.8 2006/10/16 19:42:41 dkuschel Exp $
 */

#ifndef _POS_NANO_H
#define _POS_NANO_H

#ifndef DOX
#define DOX  0
#endif



/*---------------------------------------------------------------------------
 *  NANO LAYER INSTALLATION
 *-------------------------------------------------------------------------*/

/* include configuration files */
#ifndef _PICOOS_H
#include <port.h>
#include <poscfg.h>
#endif
#include <noscfg.h>


#ifndef NOSCFG_FEATURE_REGISTRY
#define NOSCFG_FEATURE_REGISTRY  0
#endif

#if POSCFG_TASKSTACKTYPE==0
#define NOS_NEEDTASKEXITHOOK
#else
#if NOSCFG_FEATURE_REGISTRY!=0
#define NOS_NEEDTASKEXITHOOK
#endif
#endif

#ifdef NOS_NEEDTASKEXITHOOK

/* define hook */
#ifdef POSCFG_TASKEXIT_HOOK
#undef POSCFG_TASKEXIT_HOOK
#endif
#define POSCFG_TASKEXIT_HOOK  1

/* set additional task data for the nano layer */
#define NOS_TASKDATA  void *nosstkroot;

#endif /* NOS_NEEDTASKEXITHOOK */

/* include pico]OS header if not yet done */
#ifndef _PICOOS_H
#include <picoos.h>
#endif



/*---------------------------------------------------------------------------
 *  CONFIGURATION
 *-------------------------------------------------------------------------*/

#ifndef NOSCFG_FEATURE_MEMALLOC
#error  NOSCFG_FEATURE_MEMALLOC not defined
#endif
#ifndef NOSCFG_FEATURE_CONIN
#error  NOSCFG_FEATURE_CONIN not defined
#endif
#ifndef NOSCFG_FEATURE_CONOUT
#error  NOSCFG_FEATURE_CONOUT not defined
#endif
#ifndef NOSCFG_CONOUT_HANDSHAKE
#error  NOSCFG_CONOUT_HANDSHAKE not defined
#endif
#ifndef NOSCFG_CONOUT_FIFOSIZE
#error  NOSCFG_CONOUT_FIFOSIZE not defined
#endif
#ifndef NOSCFG_FEATURE_PRINTF
#error  NOSCFG_FEATURE_PRINTF not defined
#endif
#ifndef NOSCFG_FEATURE_SPRINTF
#error  NOSCFG_FEATURE_SPRINTF not defined
#endif
#ifndef NOSCFG_FEATURE_TASKCREATE
#error  NOSCFG_FEATURE_TASKCREATE not defined
#endif
#ifndef NOSCFG_DEFAULT_STACKSIZE
#error  NOSCFG_DEFAULT_STACKSIZE not defined in the port configuration file
#endif
#ifndef NOSCFG_STACK_GROWS_UP
#error  NOSCFG_STACK_GROWS_UP not defined in the port configuration file
#endif
#ifndef NOSCFG_MEM_MANAGE_MODE
#error  NOSCFG_MEM_MANAGE_MODE not defined
#endif
#ifndef NOSCFG_FEATURE_MEMSET
#error  NOSCFG_FEATURE_MEMSET not defined
#endif
#ifndef NOSCFG_FEATURE_MEMCOPY
#error  NOSCFG_FEATURE_MEMCOPY not defined
#endif
#ifndef NOSCFG_STKMEM_RESERVE
#error  NOSCFG_STKMEM_RESERVE not defined
#endif
#if NOSCFG_MEM_MANAGER_TYPE == 2
#ifndef NOSCFG_MEM_USER_MALLOC
#error  NOSCFG_MEM_USER_MALLOC not defined
#endif
#ifndef NOSCFG_MEM_USER_FREE
#error  NOSCFG_MEM_USER_FREE not defined
#endif
#endif
#ifndef NOSCFG_FEATURE_BOTTOMHALF
#error  NOSCFG_FEATURE_BOTTOMHALF not defined
#endif
#if NOSCFG_FEATURE_BOTTOMHALF != 0
#ifndef NOS_MAX_BOTTOMHALFS
#error  NOS_MAX_BOTTOMHALFS not defined
#endif
#if (NOS_MAX_BOTTOMHALFS == 0) || (NOS_MAX_BOTTOMHALFS > MVAR_BITS)
#error NOS_MAX_BOTTOMHALFS must be in the range 1 .. MVAR_BITS
#endif
#endif
#ifndef NOSCFG_FEATURE_CPUUSAGE
#error  NOSCFG_FEATURE_CPUUSAGE not defined
#endif

#if NOSCFG_FEATURE_REGISTRY
#ifndef NOSCFG_FEATURE_REGQUERY
#error  NOSCFG_FEATURE_REGQUERY
#endif
#ifndef NOSCFG_FEATURE_USERREG
#error  NOSCFG_FEATURE_USERREG
#endif
#ifndef NOS_MAX_REGKEYLEN
#error  NOS_MAX_REGKEYLEN
#endif
#if NOS_MAX_REGKEYLEN < 4
#error NOS_MAX_REGKEYLEN must be at least 4
#endif
#ifndef NOS_REGKEY_PREALLOC
#error  NOS_REGKEY_PREALLOC
#endif
#else /* NOSCFG_FEATURE_REGISTRY */
#ifdef NOSCFG_FEATURE_USERREG
#undef NOSCFG_FEATURE_USERREG
#endif
#define NOSCFG_FEATURE_USERREG 0
#ifdef NOSCFG_FEATURE_REGQUERY
#undef NOSCFG_FEATURE_REGQUERY
#endif
#define NOSCFG_FEATURE_REGQUERY 0
#endif  /* NOSCFG_FEATURE_REGISTRY */

#ifndef NOSCFG_FEATURE_SEMAPHORES
#define NOSCFG_FEATURE_SEMAPHORES  0
#endif

#if NOSCFG_FEATURE_SEMAPHORES != 0  &&  POSCFG_FEATURE_SEMAPHORES == 0
#error NOSCFG_FEATURE_SEMAPHORES enabled, but pico]OS semaphores disabled
#endif

#ifndef NOSCFG_FEATURE_MUTEXES
#define NOSCFG_FEATURE_MUTEXES  0
#endif
#if NOSCFG_FEATURE_MUTEXES != 0  &&  POSCFG_FEATURE_MUTEXES == 0
#error NOSCFG_FEATURE_MUTEXES enabled, but pico]OS mutexes disabled
#endif

#ifndef NOSCFG_FEATURE_MSGBOXES
#define NOSCFG_FEATURE_MSGBOXES  0
#endif
#if NOSCFG_FEATURE_MSGBOXES != 0  &&  POSCFG_FEATURE_MSGBOXES == 0
#error NOSCFG_FEATURE_MSGBOXES enabled, but pico]OS message boxes disabled
#endif

#ifndef NOSCFG_FEATURE_FLAGS
#define NOSCFG_FEATURE_FLAGS  0
#endif
#if NOSCFG_FEATURE_FLAGS != 0  &&  POSCFG_FEATURE_FLAGS == 0
#error NOSCFG_FEATURE_FLAGS enabled, but pico]OS flag functions disabled
#endif

#ifndef NOSCFG_FEATURE_TIMER
#define NOSCFG_FEATURE_TIMER  0
#endif
#if NOSCFG_FEATURE_TIMER != 0  &&  POSCFG_FEATURE_TIMER == 0
#error NOSCFG_FEATURE_TIMER enabled, but pico]OS timer functions disabled
#endif

#ifndef NOSCFG_MEM_OVWR_STANDARD
#define NOSCFG_MEM_OVWR_STANDARD  1
#endif
#ifndef NOSCFG_FEATURE_REALLOC
#define NOSCFG_FEATURE_REALLOC    0
#endif



/*---------------------------------------------------------------------------
 *  DATA TYPES
 *-------------------------------------------------------------------------*/

#if DOX!=0 || NOSCFG_FEATURE_BOTTOMHALF != 0
/** Bottom half function pointer.
 * @param   arg         Optional argument that was set when the
 *                      bottom half was registered with
 *                      ::nosBottomHalfRegister.
 * @param   bh          Number of the bottom half
 *                      (0 .. ::NOS_MAX_BOTTOMHALFS - 1)
 */
typedef void (*NOSBHFUNC_t)(void* arg, UVAR_t bh);
#endif



/*---------------------------------------------------------------------------
 *  MEMORY MANAGEMENT
 *-------------------------------------------------------------------------*/

/** @defgroup mem Memory Management
 * @ingroup userapin
 * 
 * <b> Note: This API is part of the nano layer </b>
 *
 * The nano layer supplies a set of memory management functions.
 * pico]OS functions are thread save, thus pico]OS can replace the
 * memory allocation functions of a runtime library that was not
 * designed for a multitasking environment.
 * @{
 */

#ifdef _N_MEM_C
#define NANOEXT
#else
#define NANOEXT extern
#endif

#if DOX!=0 || NOSCFG_FEATURE_MEMALLOC != 0

/**
 * Allocate memory from the heap.
 * This function allocates a block of memory from the heap.
 * The function is thread save, thus multiple threads can access the
 * heap without corrupting it.
 * @param   size in bytes of the memory block to allocate.
 * @return  The function returns the pointer to the new memory block
 *          on success. NULL is returned when the function failed
 *          to allocate a block with the wished size.
 * @note    ::NOSCFG_FEATURE_MEMALLOC must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemRealloc, nosMemFree, NOSCFG_MEM_MANAGER_TYPE
 */
NANOEXT void* POSCALL nosMemAlloc(UINT_t size);

/**
 * Free a block of memory to the heap.
 * This function is the counterpart to ::nosMemAlloc.
 * @param   p pointer to the memory block to free.
 * @note    ::NOSCFG_FEATURE_MEMALLOC must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemAlloc, nosMemRealloc, NOSCFG_MEM_MANAGER_TYPE
 */
NANOEXT void POSCALL nosMemFree(void *p);


#if DOX!=0 || NOSCFG_FEATURE_REALLOC != 0
/**
 * Reallocate a block of memory.
 * Sometimes it is necessary to increase or decrease the size of a
 * memory block on the heap. This function works like the usual
 * realloc(), it is optimized to keep the heap clean and unfragmented.
 * @param   memblock  pointer to the memory block on the heap.
 * @param   size  new size of the memory block (larger or smaller)
 * @return  The function returns the pointer to the new memory block
 *          on success. NULL is returned when the function failed
 *          to change the size of the block.
 * @note    ::NOSCFG_FEATURE_MEMALLOC and ::NOSCFG_FEATURE_REALLOC
 *          must be defined to 1 to have this function compiled in.
 * @note    ::nosMemRealloc reaches the best performance only when
 *          ::NOSCFG_MEM_MANAGE_MODE is set to 1
 * @sa      nosMemAlloc, nosMemFree, NOSCFG_MEM_MANAGER_TYPE
 */
NANOEXT void POSCALL *nosMemRealloc(void *memblock, UINT_t size);
#endif

/* overwrite standard memory allocation functions */
#ifndef NANOINTERNAL
#if NOSCFG_MEM_OVWR_STANDARD != 0
#ifdef malloc
#undef malloc
#endif
#ifdef calloc
#undef calloc
#endif
#ifdef free
#undef free
#endif
#define malloc  nosMemAlloc
#define calloc  (not_supported)
#define free    nosMemFree
#endif /* NOSCFG_MEM_OVWR_STANDARD */
#else /* NANOINTERNAL */
/* internal malloc/free, used by OS core and platform ports */
#if NOSCFG_MEM_MANAGER_TYPE == 0
#define NOS_MEM_ALLOC(x)   malloc((size_t)(x))
#define NOS_MEM_FREE(x)    free(x)
#elif   NOSCFG_MEM_MANAGER_TYPE == 1
void*   nos_malloc(UINT_t size);
void    nos_free(void *mp);
#define NOS_MEM_ALLOC(x)   nos_malloc(x)
#define NOS_MEM_FREE(x)    nos_free(x)
#elif   NOSCFG_MEM_MANAGER_TYPE == 2
#define NOS_MEM_ALLOC(x)   NOSCFG_MEM_USER_MALLOC(x)
#define NOS_MEM_FREE(x)    NOSCFG_MEM_USER_FREE(x)
#endif
#endif /* NANOINTERNAL */

#endif /* NOSCFG_FEATURE_MEMALLOC */

#if DOX!=0 || NOSCFG_FEATURE_MEMSET != 0

/**
 * Fill a block of memory with a special character.
 * This function works like the memset function from the
 * C runtime library.
 * @param   buf  pointer to the destination memory block
 * @param   val  character to fill into the memory block
 * @param   count  number of bytes to fill into the block
 * @note    ::NOSCFG_FEATURE_MEMSET must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemAlloc, nosMemCopy
 */
NANOEXT void POSCALL nosMemSet(void *buf, char val, UINT_t count);

#if NOSCFG_MEM_OVWR_STANDARD != 0
#ifdef memset
#undef memset
#endif
#define memset  nosMemSet
#endif

#endif /* NOSCFG_FEATURE_MEMSET */

#if DOX!=0 || NOSCFG_FEATURE_MEMCOPY != 0

/**
 * Copy a block of memory.
 * This function works like the memcpy function from the
 * C runtime library.
 * @param   dst  pointer to the destination memory block
 * @param   src  pointer to the source memory block
 * @param   count  number of bytes to copy
 * @note    ::NOSCFG_FEATURE_MEMCOPY must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosMemAlloc, nosMemSet
 */
NANOEXT void POSCALL nosMemCopy(void *dst, void *src, UINT_t count);

#if NOSCFG_MEM_OVWR_STANDARD != 0
#ifdef memcpy
#undef memcpy
#endif
#define memcpy  nosMemCopy
#endif

#endif /* NOSCFG_FEATURE_MEMCOPY */
#undef NANOEXT
/** @} */



/*---------------------------------------------------------------------------
 *  CONSOLE INPUT / OUTPUT
 *-------------------------------------------------------------------------*/

/** @defgroup conio Console Input / Output
 * @ingroup userapin
 * 
 * <b> Note: This API is part of the nano layer </b>
 *
 * The nano layer supplies a set of multitasking able console I/O functions.
 * Note that the platform port must support some basic I/O mechanisms.
 * For console output, pico]OS calls the function ::p_putchar to output
 * a single character. This function may fail when a transmitter FIFO
 * ran out of space, and the function ::c_nos_putcharReady should be called
 * when the transmitter is ready again. Input from a terminal or keyboard is
 * fet into pico]OS by calling the function ::c_nos_keyinput or by rising the
 * software interrupt number zero with the keycode as parameter. Not all
 * platform ports may support console I/O, please read the port documentation
 * for further information.@n
 * Since the nano layer supplies also a set of printf and sprintf functions,
 * you may no more need a large runtime library in some special cases.
 * @{
 */

#ifdef _N_CONIO_C
#define NANOEXT
#else
#define NANOEXT extern
#endif

#if DOX!=0 || NOSCFG_FEATURE_CONIN != 0

/**
 * Keyboard input.
 * This function is called by the architecture port to feed keyboard
 * input into the nano layer.
 * @param   key  keycode of the pressed key
 * @note    ::NOSCFG_FEATURE_CONIN must be defined to 1 
 *          to have this function compiled in.@n
 *          The alternative to the use of this function is to use
 *          software interrupt 0 to feed keyboard data into the nano layer.
 * @sa      nosKeyGet, nosKeyPressed, NOSCFG_CONIO_KEYBUFSIZE
 */
NANOEXT void POSCALL c_nos_keyinput(UVAR_t key);

/**
 * Wait and get the code of the next pressed key.
 * This function blocks until the user presses a key on the keyboard
 * and returns the code of the pressed key as result.
 * @return  ASCII-code of the pressed key
 * @note    ::NOSCFG_FEATURE_CONIN must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosKeyPressed, c_nos_keyinput, NOSCFG_CONIO_KEYBUFSIZE
 */
NANOEXT char POSCALL nosKeyGet(void);

/**
 * Test if a key was pressed.
 * This function tests if a key code is available in the keyboard buffer.
 * Even if no key is pressed yet, the function will immediately return.
 * @return  TRUE (nonzero) when a key code is available.
 * @note    ::NOSCFG_FEATURE_CONIN must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosKeyGet, c_nos_keyinput, NOSCFG_CONIO_KEYBUFSIZE
 */
NANOEXT UVAR_t POSCALL nosKeyPressed(void);

#endif  /* NOSCFG_FEATURE_CONIN */


#if DOX!=0 || NOSCFG_FEATURE_CONOUT != 0
/**
 * Print a character to the console or terminal. This function
 * must be supplied by the architecture port; it is not callable
 * by the user.
 * @param   c  character to print out.
 * @return  This function should return nonzero (=TRUE) when the
 *          character could be printed. If the function could not
 *          print the character (e.g. the FIFO of a serial line
 *          is full), this function should return zero (=FALSE).
 *          The nano layer will try to send the character later again.
 * @note    This function must not do a CR/LF conversion, a CR
 *          must result in a simple carriage return, and a LF
 *          must result in a simple line feed without returning
 *          the carriage.
 * @note    If this function returns FALSE, the platform port
 *          must do a call to ::c_nos_putcharReady when it is
 *          ready again for accepting new characters. When
 *          ::NOSCFG_CONOUT_HANDSHAKE is disabled, the return
 *          value of this function is ignored.
 * @sa      c_nos_putcharReady, c_nos_keyinput
 */
NANOEXT UVAR_t POSCALL p_putchar(char c); 
#endif


#if DOX!=0 || NOSCFG_CONOUT_HANDSHAKE != 0
/**
 * This is the optional handshake function for ::p_putchar.
 * The handshake function is usefull for console output over
 * a serial line. The function ::p_putchar will fail when the
 * transmitter FIFO is full, and the nano layer will save the
 * last character for later transmission. The platform port
 * should than call ::c_nos_putcharReady when the transmitter
 * FIFO has space again (when the last character has left the
 * output shift register); most commonly this is signalled by
 * a hardware interrupt, that would be used to call this
 * handshake function. @n
 * The purpose of this handshaking is to reduce CPU usage
 * by avoiding polling on the standard output until the stdout
 * is ready again. In the current implementation this function
 * triggers a semaphore that wakes the task waiting for
 * service on standard out. @n
 * Note that this function must be supplied by the
 * architecture port; it is not callable by the user.
 * @note To enable this handshake function, the define
 *       ::NOSCFG_CONOUT_HANDSHAKE must be set to 1.
 */
NANOEXT void POSCALL c_nos_putcharReady(void);
#endif


#if DOX!=0 || NOSCFG_FEATURE_CONOUT != 0

/**
 * Print a character to the console or terminal.
 * This function prints a single character to the console.
 * No CR/LF conversion is performed.
 * @note    ::NOSCFG_FEATURE_CONOUT must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosPrint, p_putchar
 */
NANOEXT void POSCALL nosPrintChar(char c);

/**
 * Print a character string to the console or terminal.
 * This function prints a string of characters (text) to the console.
 * A CR/LF conversion is performed, CR is preceding each LF.
 * @param   s  pointer to zero terminated ASCII string
 * @note    ::NOSCFG_FEATURE_CONOUT must be defined to 1 
 *          to have this function compiled in.
 * @sa      nosPrintChar, p_putchar
 */
NANOEXT void POSCALL nosPrint(const char *s);

#endif

#if NOSCFG_FEATURE_CONOUT == 0
#if NOSCFG_FEATURE_PRINTF != 0
#undef  NOSCFG_FEATURE_PRINTF
#define NOSCFG_FEATURE_PRINTF  0
#endif
#endif

#if NOSCFG_FEATURE_PRINTF != 0 || NOSCFG_FEATURE_SPRINTF != 0
typedef void* NOSARG_t;
#endif


#if DOX!=0 || ((NOSCFG_FEATURE_CONOUT != 0)&&(NOSCFG_FEATURE_PRINTF != 0))

NANOEXT void POSCALL n_printFormattedN(const char *fmt, NOSARG_t args);

#if DOX
/**
 * Print a formated character string to the console or terminal.
 * This function acts like the usual printf function, except that
 * it is limmited to the basic formats. The largest integer that
 * can be displayed is of type INT_t.
 * @param   fmt  format string
 * @param   a1   first argument
 * @note    ::NOSCFG_FEATURE_CONOUT and ::NOSCFG_FEATURE_PRINTF 
 *          must be defined to 1
 *          to have this function compiled in.@n
 *          This function is not variadic. To print strings with
 *          more than one argument, you may use the functions
 *          nosPrintf2 (2 arguments) to nosPrintf6 (6 arguments).
 * @sa      nosPrintChar, nosPrint
 */
NANOEXT void POSCALL nosPrintf1(const char *fmt, arg a1);

#else /* DOX!=0 */
#define nosPrintf1(fmt, a1)  \
  do { \
    NOSARG_t args[1]; args[0] = (NOSARG_t)(a1); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf2(fmt, a1, a2)  \
  do { \
    NOSARG_t args[2]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf3(fmt, a1, a2, a3)  \
  do { \
    NOSARG_t args[3]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf4(fmt, a1, a2, a3, a4)  \
  do { \
    NOSARG_t args[4]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf5(fmt, a1, a2, a3, a4, a5)  \
  do { \
    NOSARG_t args[5]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    args[3] = (NOSARG_t)(a4); args[4] = (NOSARG_t)(a5); \
    n_printFormattedN(fmt, args); \
  } while(0);

#define nosPrintf6(fmt, a1, a2, a3, a4, a5, a6)  \
  do { \
    NOSARG_t args[6]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    args[4] = (NOSARG_t)(a5); args[5] = (NOSARG_t)(a6); \
    n_printFormattedN(fmt, args); \
  } while(0);

#endif /* DOX!=0 */
#endif /* NOSCFG_FEATURE_PRINTF */


#if DOX!=0 || NOSCFG_FEATURE_SPRINTF != 0
#if DOX
/**
 * Print a formated character string to a string buffer.
 * This function acts like the usual sprintf function, except that
 * it is limmited to the basic formats. The largest integer that
 * can be displayed is of type INT_t.
 * @param   buf  destination string buffer
 * @param   fmt  format string
 * @param   a1   first argument
 * @note    ::NOSCFG_FEATURE_SPRINTF must be defined to 1 
 *          to have this function compiled in.@n
 *          This function is not variadic. To print strings with
 *          more than one argument, you may use the functions
 *          nosSPrintf2 (2 arguments) to nosSPrintf6 (6 arguments).
 * @sa      nosPrintf1, nosPrint
 */
NANOEXT void POSCALL nosSPrintf1(char *buf, const char *fmt, arg a1);

#else /* DOX!=0 */

NANOEXT void POSCALL n_sprintFormattedN(char *buf, const char *fmt,
                                        NOSARG_t args);

#define nosSPrintf1(buf, fmt, a1)  \
  do { \
    NOSARG_t args[1]; args[0] = (NOSARG_t)(a1); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf2(buf, fmt, a1, a2)  \
  do { \
    NOSARG_t args[2]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf3(buf, fmt, a1, a2, a3)  \
  do { \
    NOSARG_t args[3]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf4(buf, fmt, a1, a2, a3, a4)  \
  do { \
    NOSARG_t args[4]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf5(buf, fmt, a1, a2, a3, a4, a5)  \
  do { \
    NOSARG_t args[5]; args[0] = (NOSARG_t)(a1); \
    args[1] = (NOSARG_t)(a2); args[2] = (NOSARG_t)(a3); \
    args[3] = (NOSARG_t)(a4); args[4] = (NOSARG_t)(a5); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#define nosSPrintf6(buf, fmt, a1, a2, a3, a4, a5, a6)  \
  do { \
    NOSARG_t args[6]; \
    args[0] = (NOSARG_t)(a1); args[1] = (NOSARG_t)(a2); \
    args[2] = (NOSARG_t)(a3); args[3] = (NOSARG_t)(a4); \
    args[4] = (NOSARG_t)(a5); args[5] = (NOSARG_t)(a6); \
    n_sprintFormattedN(buf, fmt, args); \
  } while(0);

#endif /* DOX!=0 */
#endif /* NOSCFG_FEATURE_SPRINTF */
#undef NANOEXT
/** @} */



/*---------------------------------------------------------------------------
 *  BOTTOM HALFS
 *-------------------------------------------------------------------------*/

/** @defgroup bhalf Bottom Halfs
 * @ingroup userapin
 * 
 * <b> Note: This API is part of the nano layer </b>
 *
 * Interrupt service routines can be divided into to halfs: the top and
 * the bottom half. The top half is that piece of code, that is directly
 * executed when the processor gets an hardware interrupt signalled.
 * This code is usually executed with globally disabled interrupts. Thus,
 * a second interrupt that happens while the first interrupt is still
 * serviced, will be delayed until the processor has left the currently
 * running ISR again. To minimize interrupt delay, only the critical part
 * of the ISR (the top half) is executed at interrupt level, all non
 * critical code is executed at task level (bottom half). Because the bottom
 * half is interruptable, critical interrupts won't be delayed too much.
 * @{
 */

#ifdef _N_BHALF_C
#define NANOEXT
#else
#define NANOEXT extern
#endif

#if DOX!=0 || NOSCFG_FEATURE_BOTTOMHALF != 0
/**
 * Bottom half function. Registers a new bottom half.
 * @param   number      Number of the bottom half. Must be between
 *                      0 and ::NOS_MAX_BOTTOMHALFS - 1.
 * @param   func        pointer to the bottom half function that shall be
 *                      executed when the bottom half is triggered.
 * @param   arg         optional argument passed to function func.
 * @return  Zero on success. Nonzero values denote an error.
 * @note    To unregister a bottom half function again, you may call
 *          this function with funcptr = NULL, or alternatively,
 *          you can use the macro ::nosBottomHalfUnregister. @n
 *          ::NOSCFG_FEATURE_BOTTOMHALF must be defined to 1
 *          to enable bottom half support.
 * @note    Important! A bottom half function is not allowed to block,
 *          that means such a function must not call functions that
 *          may block (for example, this functions are nosTaskSleep,
 *          nosSemaGet, nosSemaWait, nosMutexLock).
 * @sa      nosBottomHalfUnregister, nosBottomHalfStart
 */
NANOEXT VAR_t POSCALL nosBottomHalfRegister(UVAR_t number, NOSBHFUNC_t func,
                                            void *arg);

/**
 * Bottom half function. Unregisters a bottom half.
 * @param   number      Number of the bottom half to unregister.
 *                      Must be between 0 and ::NOS_MAX_BOTTOMHALFS - 1.
 * @return  Zero on success. Nonzero values denote an error.
 * @note    ::NOSCFG_FEATURE_BOTTOMHALF must be defined to 1
 *          to enable bottom half support.
 * @sa      nosBottomHalfRegister
 */
#define nosBottomHalfUnregister(number) \
          nosBottomHalfRegister(number, NULL, NULL)

/**
 * Bottom half function. Triggers a bottom half function.
 * The bottom half is executed when the interrupt level is left.
 * @param   number      Number of the bottom half. Must be between
 *                      0 and ::NOS_MAX_BOTTOMHALFS - 1.
 * @note    This function is called by the top half of an
 *          interrupt service routine. The ISR that calls this function
 *          does not need to call ::c_pos_intEnter before. @n
 *          ::NOSCFG_FEATURE_BOTTOMHALF must be defined to 1
 *          to enable bottom half support.
 * @sa      nosBottomHalfRegister, nosBottomHalfUnregister
 */
NANOEXT void POSCALL nosBottomHalfStart(UVAR_t number);

#endif /* NOSCFG_FEATURE_BOTTOMHALF */
#undef NANOEXT
/** @} */



/*---------------------------------------------------------------------------
 *  REGISTRY
 *-------------------------------------------------------------------------*/

/** @defgroup registry Registry
 * @ingroup userapin
 * 
 * <b> Note: This API is part of the nano layer </b>
 *
 * Registry keys are short ASCII texts that are assigned to binary numbers
 * like integers or pointers. pico]OS uses registry keys in two ways: @n@n
 * First, this keys are used to identify resources, such as tasks,
 * semaphores and timers. So it is possible to create named semaphores,
 * that are accessible by every program module that knows the name of
 * the semaphore (the program module does not need to know the exact
 * semaphore handle, the ASCII name is sufficiennt). Also with named
 * resources it is possible to maintain a list of allocated resources,
 * e.g. this resource list can be printed out to a shell window. @n@n
 * Second, registry keys can be used by an application to maintain a
 * central storage with setup and configuration data. This is known
 * as "the registry" in MS Windows operating systems.
 * @{
 */

#ifdef _N_REG_C
#define NANOEXT
#else
#define NANOEXT extern
#endif

#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0

/** Generic Handle */
typedef void*  NOSGENERICHANDLE_t;

/** Registry Query Handle. Every registry query uses an own handle
    to the registry system.*/
typedef void*  NOSREGQHANDLE_t;

/* Generic registry key value type.
   It is on the developer how he uses the type. */
typedef union {
  void*       voidptr; /*!< pointer type, may be used for handles */
  int         integer; /*!< integer type, may be used to store numbers */
} KEYVALUE_t;

/** Registry type */
typedef enum {
  REGTYPE_TASK = 0,    /*!< task registry */
#if DOX!=0 || NOSCFG_FEATURE_SEMAPHORES != 0
  REGTYPE_SEMAPHORE,   /*!< semaphore registry */
#endif
#if DOX!=0 || NOSCFG_FEATURE_MUTEXES != 0
  REGTYPE_MUTEX,       /*!< mutex registry */
#endif
#if DOX!=0 || NOSCFG_FEATURE_FLAGS != 0
  REGTYPE_FLAG,        /*!< flag event registry */
#endif
#if DOX!=0 || NOSCFG_FEATURE_TIMER != 0
  REGTYPE_TIMER,       /*!< timer registry */
#endif
#if DOX!=0 || NOSCFG_FEATURE_USERREG != 0
  REGTYPE_USER,        /*!< user defined registry */
#endif
  REGTYPE_SEARCHALL    /*!< this is a special flag for the
                            function nosGetNameByHandle */
} NOSREGTYPE_t;
#define MIN_REGTYPE  REGTYPE_TASK
#define MAX_REGTYPE  (REGTYPE_SEARCHALL-1)


/**
 * Registry function. Searches the registry for an object name and returns
 * the handle that is assigned to the object.
 * For example, somebody can get a semaphore handle by just knowing the
 * semaphores name.
 * @param objtype   Type of the object that is searched for. Valid types are:
 *                  REGTYPE_TASK, REGTYPE_SEMAPHORE, REGTYPE_MUTEX,
 *                  REGTYPE_FLAG, REGTYPE_TIMER, REGTYPE_USER
 * @param objname   Name of the object to search for.
 * @return  The handle to the object on success,
 *          NULL if the object was not found.
 * @note    ::NOSCFG_FEATURE_REGISTRY must be defined to 1 to enable
 *          the registry and this function.@n
 * @sa nosGetNameByHandle
 */
NANOEXT NOSGENERICHANDLE_t POSCALL nosGetHandleByName(
                                 NOSREGTYPE_t objtype, const char *objname);


/**
 * Registry function. Searches the registry for a handle and returns the
 * name of the objetct.
 * @param handle    Object-handle to search the registry for.
 * @param buffer    If the object could be found in the registry,
 *                  the name of the object will be stored in this buffer.
 * @param bufsize   Size of the buffer in bytes. The size should be at
 *                  least ::NOS_MAX_REGKEYLEN + 1.
 * @param what      What to search for. If the type of the handle is known,
 *                  this parameter should be set to
 *                  REGTYPE_TASK, REGTYPE_SEMAPHORE, REGTYPE_MUTEX,
 *                  REGTYPE_FLAG, REGTYPE_TIMER or REGTYPE_USER.
 *                  If the object type is unknown, you may specify
 *                  REGTYPE_SEARCHALL. But note that the user branch of
 *                  the registry will not be included into the search.
 * @note    ::NOSCFG_FEATURE_REGISTRY must be defined to 1 to enable
 *          the registry and this function.@n
 * @sa nosGetHandleByName
 */
NANOEXT VAR_t POSCALL nosGetNameByHandle(NOSGENERICHANDLE_t handle,
                                         char *buffer, VAR_t bufsize,
                                         NOSREGTYPE_t what);

#if DOX!=0  ||  NOSCFG_FEATURE_USERREG != 0
/**
 * Registry function. Sets a key value or creates a new registry key string.
 * This function is used to assign a binary value to a text string.
 * If the user knows the text string, he can call ::nosRegGet to
 * get the binary value that is associated with the text string.
 * @param   keyname   text string, name of the registry key to create or set
 * @param   keyvalue  binary value that shall be assigned to the registry key
 * @return  Zero on success. Nonzero values denote an error.
 * @note    When creating a new registry key string, you can use the asteriks
 *          joker sign as last character in the registry key string. This
 *          function will replace the asteriks character by a decimal number,
 *          so that the generated registry key will be unique. @n
 *          ::NOSCFG_FEATURE_REGISTRY and ::NOSCFG_FEATURE_USERREG
 *          must be defined to 1 to enable the registry and this function.@n
 *          The maximum length of a registry key string is ::NOS_MAX_REGKEYLEN.
 * @sa nosRegGet, nosRegDel
 */
NANOEXT VAR_t POSCALL nosRegSet(const char *keyname, KEYVALUE_t keyvalue);

/**
 * Registry function. Returns the binary value that is assigned to a
 * registry key. This function is the counterpart to function ::nosRegSet.
 * @param   keyname     text string that binary value shall be returned
 * @param   keyvalue    Pointer to a variable of type KEYVALUE_t.
 *                      When the function succeeds (the text string
 *                      could be found), the value that is associated
 *                      with the registry key is stored in this variable.
 * @return  Zero on success. Nonzero values denote an error.
 * @note    ::NOSCFG_FEATURE_REGISTRY and ::NOSCFG_FEATURE_USERREG
 *          must be defined to 1 to enable the registry and this function.
 * @sa nosRegSet, nosRegDel
 */
NANOEXT VAR_t POSCALL nosRegGet(const char *keyname, KEYVALUE_t *keyvalue);

/**
 * Registry function. Deletes a registry key string.
 * @param   keyname     Name of the registry key string to delete.
 * @return  Zero on success. Nonzero values denote an error.
 * @note    ::NOSCFG_FEATURE_REGISTRY and ::NOSCFG_FEATURE_USERREG
 *          must be defined to 1 to enable the registry and this function.
 * @sa nosRegSet, nosRegGet
 */
NANOEXT VAR_t POSCALL nosRegDel(const char *keyname);
#endif

#if DOX!=0 || NOSCFG_FEATURE_REGQUERY != 0
/**
 * Registry function. Queries a list of registry keys.
 * This function starts a new registry query.
 * @param   type        Type of the registry to query:
 *                      - REGTYPE_TASK:      query list of task handles
 *                      - REGTYPE_SEMAPHORE: query list of semaphore handles
 *                      - REGTYPE_MUTEX:     query list of mutex handles
 *                      - REGTYPE_FLAG:      query list of flag event handles
 *                      - REGTYPE_TIMER:     query list of timer handles
 *                      - REGTYPE_USER:  query list of user values (registry)
 * @return  Handle to the new query. NULL is returned on error.
 * @note    In the current implementation, only one registry query can run
 *          at a time. The next query can start when the first query
 *          is finnished (function ::nosRegQueryEnd called). @n
 *          As long as the user queries the registry, all other operating system
 *          functions that try to access the registry will be blocked. @n
 *          ::NOSCFG_FEATURE_REGISTRY and ::NOSCFG_FEATURE_REGQUERY
 *          must be defined to 1 to enable the registry and this function.
 * @sa nosRegQueryElem, nosRegQueryEnd
 */
NANOEXT NOSREGQHANDLE_t POSCALL nosRegQueryBegin(NOSREGTYPE_t type);

/**
 * Registry function. Returns the next found element in a query.
 * @param   qh          Handle to the current query 
 *                      (returnvalue of ::nosRegQueryBegin).
 * @param   genh        Pointer to a (user provided) generic handle variable.
 *                      In this variable the handle of the next found
 *                      registry key will be stored. For user registry keys
 *                      (REGTYPE_USER), this is the KEYVALUE_t.voidptr
 * @param   namebuf     Pointer to a (user provided) character buffer.
 *                      The buffer is filled with the name of the next
 *                      registry key that is found. The buffer must have
 *                      at least a size of NOS_MAX_REGKEYLEN+1 characters.
 * @param   bufsize     Size of namebuf in bytes.
 * @return  Zero on success (E_OK). A negative value denotes an error.
 *          -E_NOMORE is returned when the end of the query is reached.
 * @note    ::NOSCFG_FEATURE_REGISTRY and ::NOSCFG_FEATURE_REGQUERY
 *          must be defined to 1 to enable the registry and this function.
 * @sa nosRegQueryBegin, nosRegQueryEnd
 */
NANOEXT VAR_t POSCALL nosRegQueryElem(NOSREGQHANDLE_t qh,
                                      NOSGENERICHANDLE_t *genh,
                                      char* namebuf, VAR_t bufsize);

/**
 * Registry function. Finnishes a query.
 * This function is the counterpart to ::nosRegQueryBegin.
 * @param   qh          Handle to the current query 
 *                      (returnvalue of ::nosRegQueryBegin).
 * @return  Zero on success. A negative value denotes an error.
 * @note    ::NOSCFG_FEATURE_REGISTRY and ::NOSCFG_FEATURE_REGQUERY
 *          must be defined to 1 to enable the registry and this function.
 * @sa nosRegQueryBegin, nosRegQueryElem
 */
NANOEXT void POSCALL nosRegQueryEnd(NOSREGQHANDLE_t qh);
#endif

#endif /* NOSCFG_FEATURE_REGISTRY */
#undef NANOEXT
/** @} */



/*---------------------------------------------------------------------------
 *  CPU USAGE
 *-------------------------------------------------------------------------*/

#if (DOX!=0) || (NOSCFG_FEATURE_CPUUSAGE != 0)
/** @defgroup cpuusage CPU Usage Calculation
 * @ingroup userapin
 * The nano layer features CPU usage measurement. If this feature is
 * enabled, the system start is delayed for approximately one second that
 * is needed to calibrate the idle loop counter.
 * The CPU usage statistics is updated one time per second.
 * @{
 */
/**
 * Calculate and return the percentage of CPU usage.
 * @return  percentage of CPU usage (0 ... 100 %%)
 * @note    ::NOSCFG_FEATURE_CPUUSAGE must be defined to 1
 *          to have this function compiled in.
 */
UVAR_t POSCALL nosCpuUsage(void);
#endif
/** @} */



/*---------------------------------------------------------------------------
 *  ABSTRACTED FUNCTIONS
 *-------------------------------------------------------------------------*/

#ifdef _N_CORE_C
#define NANOEXT
#else
#define NANOEXT extern
#endif

/** @defgroup absfunc Abstracted Functions
 * @ingroup userapin
 * The nano layer supports most of all pico layer functions in an
 * abstracted form. This is necessary to follow the layering scheme
 * while all pico layer functions remain accessible.
 * The general rule is to replace the prefix 'pos' by the new prefix 'nos'
 * for the nano layer functions. But pay attention: Some nano layer functions
 * have some more functionality as their pendant in the pico layer.
 * @{
 */

/** @defgroup nanotask Task Control Functions
 * @ingroup absfunc
 * @{
 */

/** Handle to a nano layer task object. */
typedef  POSTASK_t  NOSTASK_t;


#if (DOX!=0) || (NOSCFG_FEATURE_TASKCREATE != 0)
/**
 * Generic task function. Creates a new task.
 * @param   funcptr    pointer to the function that shall be executed
 *                     by the new task.
 * @param   funcarg    optional argument passed to function funcptr.
 * @param   priority   task priority. Must be in the range
 *                     0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                     The higher the number, the higher the priority.
 * @param   stacksize  Size of the stack memory. If set to zero,
 *                     a default stack size is assumed
 *                     (see define ::NOSCFG_DEFAULT_STACKSIZE).
 * @param   name       Name of the new task to create. If the last character
 *                     in the name is an asteriks (*), the operating system
 *                     automatically assigns the task a unique name (the
 *                     registry feature must be enabled for this automatism).
 *                     This parameter can be NULL if the nano layer registry
 *                     feature is not used and will not be used in future.
 * @return  handle to the task. NULL is returned when the
 *          task could not be created.
 * @note    ::NOSCFG_FEATURE_TASKCREATE must be defined to 1
 *          to have this function compiled in.
 * @sa      nosTaskExit
 */
NANOEXT NOSTASK_t POSCALL nosTaskCreate(POSTASKFUNC_t funcptr,
                                        void *funcarg,
                                        VAR_t priority,
                                        UINT_t stacksize,
                                        const char* name);


#if (DOX!=0) || (POSCFG_FEATURE_YIELD != 0)
/**
 * Task function.
 * This function can be called to give off processing time so other tasks
 * ready to run will be scheduled (= cooparative multitasking).
 * @note    ::POSCFG_FEATURE_YIELD must be defined to 1
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskSleep
 */
#if DOX
NANOEXT void POSCALL nosTaskYield(void);
#else
#define nosTaskYield()  posTaskYield()
#endif
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
 *          Sleeping a very short time is inaccurate. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskYield, HZ, MS
 */
#if DOX
NANOEXT void POSCALL nosTaskSleep(UINT_t ticks);
#else
#define nosTaskSleep(ticks)  posTaskSleep(ticks)
#endif
#endif

#if (DOX!=0) || (POSCFG_FEATURE_EXIT != 0)
/**
 * Task function.
 * Terminate execution of a task.
 * @note    ::POSCFG_FEATURE_EXIT must be defined to 1 
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskCreate
 */
#if DOX
NANOEXT void POSCALL nosTaskExit(void);
#else
#define nosTaskExit()  posTaskExit()
#endif
#endif

#if (DOX!=0) || (POSCFG_FEATURE_GETTASK != 0)
/**
 * Task function.
 * Get the handle to the currently running task.
 * @return  the task handle.
 * @note    ::POSCFG_FEATURE_GETTASK must be defined to 1 
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskCreate, nosTaskSetPriority
 */
#if DOX
NANOEXT NOSTASK_t POSCALL nosTaskGetCurrent(void);
#else
#define nosTaskGetCurrent()  (NOSTASK_t)posTaskGetCurrent()
#endif
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
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskCreate, nosTaskExit
 */
#if DOX
NANOEXT VAR_t POSCALL nosTaskUnused(NOSTASK_t taskhandle);
#else
#define nosTaskUnused(th)  posTaskUnused((POSTASK_t)(th))
#endif
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
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskGetPriority, nosTaskGetCurrent, nosTaskCreate
 */
#if DOX
NANOEXT VAR_t POSCALL nosTaskSetPriority(NOSTASK_t taskhandle,
                                         VAR_t priority);
#else
#define nosTaskSetPriority(th, prio) posTaskSetPriority((POSTASK_t)(th),prio)
#endif
#endif

#if (DOX!=0) || (POSCFG_FEATURE_GETPRIORITY != 0)
/**
 * Task function.
 * Get the priority of a task.
 * @param   taskhandle  handle to the task.
 * @return  the priority of the task. A negative value is returned on error.
 * @note    ::POSCFG_FEATURE_GETPRIORITY must be defined to 1 
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskSetPriority, nosTaskGetCurrent, nosTaskCreate
 */
#if DOX
NANOEXT VAR_t POSCALL nosTaskGetPriority(NOSTASK_t taskhandle);
#else
#define nosTaskGetPriority(th)  posTaskGetPriority((POSTASK_t)(th))
#endif
#endif

#if (DOX!=0) || (POSCFG_FEATURE_INHIBITSCHED != 0)
/**
 * Task function.
 * Locks the scheduler. When this function is called, no task switches
 * will be done any more, until the counterpart function ::nosTaskSchedUnlock
 * is called. This function is usefull for short critical sections that
 * require exclusive access to variables. Note that interrupts still
 * remain enabled.
 * @note    ::POSCFG_FEATURE_INHIBITSCHED must be defined to 1 
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskSchedUnlock
 */
#if DOX
NANOEXT void POSCALL nosTaskSchedLock(void);
#else
#define nosTaskSchedLock()  posTaskSchedLock()
#endif

/**
 * Task function.
 * Unlocks the scheduler. This function is called to leave a critical section.
 * If a context switch request is pending, the context switch will happen
 * directly after calling this function.
 * @note    ::POSCFG_FEATURE_INHIBITSCHED must be defined to 1 
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTaskSchedLock
 */
#if DOX
NANOEXT void POSCALL nosTaskSchedUnlock(void);
#else
#define nosTaskSchedUnlock()  posTaskSchedUnlock()
#endif
#endif

#if (DOX!=0) || (POSCFG_TASKCB_USERSPACE > 0)
/**
 * Task function.
 * Returns a pointer to the user memory in the current task control block.
 * @note    ::POSCFG_TASKCB_USERSPACE must be defined to a nonzero value
 *          to have this function compiled in. ::POSCFG_TASKCB_USERSPACE
 *          is also used to set the size of the user memory (in bytes).@n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @return  pointer to user memory space.
 */
#if DOX
NANOEXT void* POSCALL nosTaskGetUserspace(void);
#else
#define nosTaskGetUserspace()  posTaskGetUserspace()
#endif
#endif

#if (DOX!=0) || (POSCFG_FEATURE_IDLETASKHOOK != 0)
/** Idle task function pointer */
typedef POSIDLEFUNC_t NOSIDLEFUNC_t;
/**
 * Task function.
 * Install or remove an optional idle task hook function.
 * The hook function is called every time the system is idle.
 * It is possible to use this hook to implement your own idle task;
 * in this case the function does not need to return to the system.
 * You may insert a call to ::nosTaskYield into your idle task loop
 * to get a better task performance.
 * @param   idlefunc  function pointer to the new idle task handler.
 *                    If this parameter is set to NULL, the idle
 *                    task function hook is removed again.
 * @return  This function may return a pointer to the last hook
 *          function set. If so (pointer is not NULL), the previous
 *          hook function should be called from within your
 *          idle task hook. This enables chaining of hook functions.
 * @note    ::POSCFG_FEATURE_IDLETASKHOOK must be defined to 1 
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 */
#if DOX
NANOEXT NOSIDLEFUNC_t POSCALL nosInstallIdleTaskHook(NOSIDLEFUNC_t idlefunc);
#else
#define nosInstallIdleTaskHook(ifunc)  posInstallIdleTaskHook(ifunc)
#endif
#endif

#endif
/** @} */

/** @defgroup nanosema Semaphore Functions
 * @ingroup absfunc
 * For detailed information about using semaphores please see
 * <a href="group__sema.html#_details">detailed description of semaphores</a>
 * @{
 */
#if DOX!=0 || NOSCFG_FEATURE_SEMAPHORES != 0

/** Handle to a nano layer semaphore object. */
typedef  POSSEMA_t  NOSSEMA_t;

/**
 * Semaphore function.
 * Allocates a new semaphore object.
 * @param   initcount Initial semaphore count (see detailed semaphore
 *                    description in pico laye documentation).
 * @param   options   Currently unused. Please set this parameter to 0 (zero).
 * @param   name      Name of the new semaphore object to create. If the last
 *                    character in the name is an asteriks (*), the operating
 *                    system automatically assigns the semaphore a unique
 *                    name (the registry feature must be enabled for this
 *                    automatism). This parameter can be NULL if the nano
 *                    layer registry feature is not used and will not be
 *                    used in future.
 * @return  the pointer to the new semaphore object. NULL is returned on error.
 * @note    ::NOSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in. @n
 *          You must use ::nosSemaDestroy to destroy the semaphore again.@n
 *          Even if the function posSemaDestroy would work also, it is
 *          required to call ::nosSemaDestroy. Only this function removes
 *          the semaphore from the registry. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosSemaDestroy, nosSemaGet, nosSemaWait, nosSemaSignal
 */
#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0
NANOEXT NOSSEMA_t POSCALL nosSemaCreate(INT_t initcount, UVAR_t options,
                                        const char *name);
#else
#define nosSemaCreate(i, o, n)  (NOSSEMA_t) posSemaCreate(i);
#endif

#if DOX!=0 || POSCFG_FEATURE_SEMADESTROY != 0
/**
 * Semaphore function.
 * Frees a no more needed semaphore object.
 * @param   sema  handle to the semaphore object.
 * @note    ::NOSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in.@n
 *          ::POSCFG_FEATURE_SEMADESTROY must be defined to 1
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosSemaCreate
 */
#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0
NANOEXT void POSCALL nosSemaDestroy(NOSSEMA_t sema);
#else
#define nosSemaDestroy(sema)  posSemaDestroy((POSSEMA_t)(sema))
#endif
#endif

#if DOX
/**
 * Semaphore function.
 * This function signalizes a semaphore object, that means it increments
 * the semaphore counter and sets tasks pending on the semaphore to 
 * running state, when the counter reaches a positive, nonzero value.
 * @param   sema  handle to the semaphore object.
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosSemaGet, nosSemaWait, nosSemaCreate
 */
NANOEXT VAR_t POSCALL nosSemaSignal(NOSSEMA_t sema);
#else
#define nosSemaSignal(sem)  posSemaSignal((POSSEMA_t)(sem))
#endif

#if DOX
/**
 * Semaphore function.
 * This function tries to get the semaphore object. If the semaphore
 * is in nonsignalized state (that means its counter is zero or less),
 * this function blocks the task execution until the semaphore
 * gets signaled.
 * @param   sema  handle to the semaphore object.
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosSemaWait, nosSemaSignal, nosSemaCreate
 */
NANOEXT VAR_t POSCALL nosSemaGet(NOSSEMA_t sema);
#else
#define nosSemaGet(sem)  posSemaGet((POSSEMA_t)(sem))
#endif

#if DOX
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
 * @note    ::NOSCFG_FEATURE_SEMAPHORES must be defined to 1 
 *          to have semaphore support compiled in.@n
 *          ::POSCFG_FEATURE_SEMAWAIT must be defined to 1
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosSemaGet, nosSemaSignal, nosSemaCreate, HZ, MS
 */
NANOEXT VAR_t POSCALL nosSemaWait(NOSSEMA_t sema, UINT_t timeoutticks);
#else
#if POSCFG_FEATURE_SEMAWAIT
#define nosSemaWait(sem, to)  posSemaWait((POSSEMA_t)(sem), to)
#endif
#endif

#endif /* POSCFG_FEATURE_SEMAPHORES */
/** @} */

/** @defgroup nanomutex Mutex Functions
 * @ingroup absfunc
 * For detailed information about using mutexes please see
 * <a href="group__mutex.html#_details">detailed description of mutexes</a>
 * @{
 */
#if DOX!=0 || NOSCFG_FEATURE_MUTEXES != 0

/** Handle to a nano layer semaphore object. */
typedef  POSMUTEX_t  NOSMUTEX_t;

/**
 * Mutex function.
 * Allocates a new mutex object.
 * @param   options   Currently unused. Please set this parameter to 0 (zero).
 * @param   name      Name of the new mutex object to create. If the last
 *                    character in the name is an asteriks (*), the operating
 *                    system automatically assigns the mutex a unique
 *                    name (the registry feature must be enabled for this
 *                    automatism). This parameter can be NULL if the nano
 *                    layer registry feature is not used and will not
 *                    be used in future.
 * @return  the pointer to the new mutex object. NULL is returned on error.
 * @note    ::NOSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in. @n
 *          Even if the function posMutexDestroy would work also, it is
 *          required to call ::nosMutexDestroy. Only this function removes
 *          the mutex from the registry. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMutexDestroy, nosMutexLock, nosMutexTryLock, nosMutexUnlock
 */
#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0
NANOEXT NOSMUTEX_t POSCALL nosMutexCreate(UVAR_t options, const char *name);
#else
#define nosMutexCreate(opt, name)  (NOSMUTEX_t) posMutexCreate()
#endif

#if DOX!=0 || POSCFG_FEATURE_MUTEXDESTROY != 0
/**
 * Mutex function.
 * Frees a no more needed mutex object.
 * @param   mutex  handle to the mutex object.
 * @note    ::NOSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in.@n
 *          ::POSCFG_FEATURE_MUTEXDESTROY must be defined to 1
 *          to have this function compiled in.@n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMutexCreate
 */
#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0
NANOEXT void POSCALL nosMutexDestroy(NOSMUTEX_t mutex);
#else
#define nosMutexDestroy(mutex)  posMutexDestroy((POSMUTEX_t)(mutex))
#endif
#endif

#if DOX!=0 || POSCFG_FEATURE_MUTEXTRYLOCK != 0
/**
 * Mutex function.
 * Tries to get the mutex lock. This function does not block when the
 * mutex is not available, instead it returns a value showing that
 * the mutex could not be locked.
 * @param   mutex  handle to the mutex object.
 * @return  zero when the mutex lock could be set. Otherwise, when
 *          the mutex lock is yet helt by an other task, the function
 *          returns 1. A negative value is returned on error.
 * @note    ::NOSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in.@n
 *          ::POSCFG_FEATURE_MUTEXTRYLOCK must be defined to 1
 *          to have this function compiled in.@n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMutexLock, nosMutexUnlock, nosMutexCreate
 */
#if DOX
NANOEXT VAR_t POSCALL nosMutexTryLock(NOSMUTEX_t mutex);
#else
#define nosMutexTryLock(mutex)  posMutexTryLock((POSMUTEX_t)(mutex))
#endif
#endif

/**
 * Mutex function.
 * This function locks a code section so that only one task can execute
 * the code at a time. If an other task already has the lock, the task
 * requesting the lock will be blocked until the mutex is unlocked again.
 * Note that a ::nosMutexLock appears always in a pair with ::nosMutexUnlock.
 * @param   mutex  handle to the mutex object.
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMutexTryLock, nosMutexUnlock, nosMutexCreate
 */
#if DOX
NANOEXT VAR_t POSCALL nosMutexLock(NOSMUTEX_t mutex);
#else
#define nosMutexLock(mutex)  posMutexLock((POSMUTEX_t)(mutex))
#endif

/**
 * Mutex function.
 * This function unlocks a section of code so that other tasks
 * are able to execute it.
 * @param   mutex  handle to the mutex object.
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_MUTEXES must be defined to 1 
 *          to have mutex support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMutexLock, nosMutexTryLock, nosMutexCreate
 */
#if DOX
NANOEXT VAR_t POSCALL nosMutexUnlock(NOSMUTEX_t mutex);
#else
#define nosMutexUnlock(mutex)  posMutexUnlock((POSMUTEX_t)(mutex))
#endif

#endif /* NOSCFG_FEATURE_MUTEXES */
/** @} */


/** @defgroup nanomsg Message Box Functions
 * @ingroup absfunc
 * For detailed information about using message boxes please see
 * <a href="group__msg.html#_details">detailed description of message boxes</a>
 * @{
 */

#if DOX!=0 || NOSCFG_FEATURE_MSGBOXES != 0

/**
 * Message box function.
 * Allocates a new message buffer. To increase the execution speed,
 * it is recommended to set ::POSCFG_MSG_MEMORY to 1. Otherwise,
 * ::nosMessageAlloc will need to call ::nosMemAlloc to allocate memory
 * (and this is possibly slower than the pico]OS internal message allocator).
 * @n Usually the sending task would allocate a new message buffer, fill
 * in its data and send it via ::nosMessageSend to the receiving task.
 * The receiving task is responsible for freeing the message buffer again.
 * @param   msgSize   size of the requested message buffer in bytes.
 * @return  the pointer to the new buffer. NULL is returned if the
 *          system is low on memory or the requested msgSize is larger
 *          than ::POSCFG_MSG_BUFSIZE (only if ::POSCFG_MSG_MEMORY is
 *          set to 1).
 * @note    ::NOSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.@n
 *          If ::POSCFG_MSG_MEMORY is set to 0, you also need to
 *          enable the nano layer memory manager by setting
 *          ::NOSCFG_FEATURE_MEMALLOC to 1.
 * @sa      nosMessageSend, nosMessageGet, nosMessageFree
 */
NANOEXT void* POSCALL nosMessageAlloc(UINT_t msgSize);

/**
 * Message box function. Frees a message buffer again.
 * Usually the receiving task would call this function after
 * it has processed a message to free the message buffer again.
 * @param   buf  Pointer to the message buffer that is no more used.
 * @note    ::NOSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.
 * @sa      nosMessageGet, nosMessageSend, nosMessageAlloc
 */
NANOEXT void POSCALL nosMessageFree(void *buf);

/**
 * Message box function. Sends a message to a task.
 * @param   buf  Pointer to the message to send.
 *               The message buffer must have been allocated by
 *               calling ::nosMessageAlloc before.
 * @param   taskhandle  handle to the task to send the message to.
 * @return  zero on success. When an error condition exist, a
 *          negative value is returned and the message buffer is freed.
 * @note    ::NOSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.
 * @sa      nosMessageAlloc, nosMessageGet
 */
NANOEXT VAR_t POSCALL nosMessageSend(void *buf, NOSTASK_t taskhandle);

/**
 * Message box function. Gets a new message from the message box.
 * If no message is available, the task blocks until a new message
 * is received.
 * @return  Pointer to the received message. Note that the
 *          message memory must be freed again with ::nosMessageFree
 *          when the message buffer is no more used.
 * @note    ::NOSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMessageFree, nosMessageAvailable,
 *          nosMessageWait, nosMessageSend
 */
#if DOX
NANOEXT void* POSCALL nosMessageGet(void);
#else
#define  nosMessageGet()  posMessageGet()
#endif

#if DOX!=0 || POSCFG_FEATURE_MSGWAIT != 0
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
 * @return  Pointer to the received message. Note that the
 *          message memory must be freed again with ::nosMessageFree
 *          when the message buffer is no more used.
 *          NULL is returned when no message was received
 *          within the specified time (=timeout).
 * @note    ::NOSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in.@n
 *          ::POSCFG_FEATURE_MSGWAIT must be defined to 1
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMessageFree, nosMessageGet, nosMessageAvailable,
 *          nosMessageSend, HZ, MS
 */
#if DOX
NANOEXT void* POSCALL nosMessageWait(UINT_t timeoutticks);
#else
#define nosMessageWait(to)  posMessageWait(to)
#endif
#endif

/**
 * Message box function.
 * Tests if a new message is available
 * in the message box. This function can be used to prevent
 * the task from blocking.
 * @return  1 (=true) when a new message is available.
 *          Otherwise zero is returned. A negative value
 *          is returned on error.
 * @note    ::NOSCFG_FEATURE_MSGBOXES must be defined to 1 
 *          to have message box support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosMessageGet, nosMessageWait
 */
#if DOX
NANOEXT VAR_t POSCALL nosMessageAvailable(void);
#else
#define nosMessageAvailable()  posMessageAvailable()
#endif

#endif  /* NOSCFG_FEATURE_MSGBOXES */
/** @} */

/** @defgroup nanoflag Flag Functions
 * @ingroup absfunc
 * For detailed information about using flags please see
 * <a href="group__flag.html#_details">detailed description of flags</a>
 * @{
 */

#if DOX!=0 || NOSCFG_FEATURE_FLAGS != 0

/** Handle to a nano layer flag object. */
typedef  POSFLAG_t  NOSFLAG_t;

/**
 * Flag function.
 * Allocates a flag object. A flag object behaves like an array of
 * one bit semaphores. The object can hold up to ::MVAR_BITS - 1 flags.
 * The flags can be used to simulate events, so a single thread can wait
 * for several events simultaneously.
 * @param   name      Name of the new flag object to create. If the last
 *                    character in the name is an asteriks (*), the operating
 *                    system automatically assigns the flag an unique name.
 *                    name (the registry feature must be enabled for this
 *                    automatism). This parameter can be NULL if the nano
 *                    layer registry feature is not used and will not be
 *                    used in future.
 * @return  handle to the new flag object. NULL is returned on error.
 * @note    ::NOSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in. @n
 *          You must use ::nosFlagDestroy to destroy the flag object again.@n
 *          Even if the function posFlagDestroy would work also, it is
 *          required to call ::nosFlagDestroy. Only this function removes
 *          the flag object from the registry. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosFlagGet, nosFlagSet, nosFlagDestroy
 */
#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0
NANOEXT NOSFLAG_t POSCALL nosFlagCreate(const char* name);
#else
#define nosFlagCreate(name)  (NOSFLAG_t) posFlagCreate()
#endif

#if DOX!=0 || POSCFG_FEATURE_FLAGDESTROY != 0
/**
 * Flag function.
 * Frees an unused flag object again.
 * @param   flg  handle to the flag object.
 * @note    ::NOSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in.@n
 *          ::POSCFG_FEATURE_FLAGDESTROY must be defined to 1
 *          to have this function compiled in.@n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosFlagCreate
 */
#if (DOX!=0) || (NOSCFG_FEATURE_REGISTRY != 0)
NANOEXT void POSCALL nosFlagDestroy(NOSFLAG_t flg);
#else
#define nosFlagDestroy(flg)  posFlagDestroy((POSFLAG_t)(flg))
#endif
#endif

/**
 * Flag function.
 * Sets a flag bit in the flag object and sets the task that
 * pends on the flag object to running state.
 * @param   flg     Handle to the flag object.
 * @param   flgnum  Number of the flag to set. The flag number
 *                  must be in the range of 0 .. ::MVAR_BITS - 2.
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosFlagCreate, nosFlagGet, nosFlagWait
 */
#if DOX
NANOEXT VAR_t POSCALL nosFlagSet(NOSFLAG_t flg, UVAR_t flgnum);
#else
#define nosFlagSet(flg, num)  posFlagSet((POSFLAG_t)(flg), num)
#endif

/**
 * Flag function.
 * Pends on a flag object and waits until one of the flags 
 * in the flag object is set.
 * @param   flg   Handle to the flag object.
 * @param   mode  can be NOSFLAG_MODE_GETSINGLE or NOSFLAG_MODE_GETMASK.
 * @return  the number of the next flag that is set when mode is set
 *          to NOSFLAG_MODE_GETSINGLE. When mode is set to 
 *          NOSFLAG_MODE_GETMASK, a bit mask with all set flags is
 *          returned. A negative value is returned on error.
 * @note    ::NOSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosFlagCreate, nosFlagSet, nosFlagWait
 */
#if DOX
NANOEXT VAR_t POSCALL nosFlagGet(NOSFLAG_t flg, UVAR_t mode);
#else
#define nosFlagGet(flg, mode)  posFlagGet((POSFLAG_t)(flg), mode)
#endif

#if DOX!=0 || POSCFG_FEATURE_FLAGWAIT != 0
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
 * @note    ::NOSCFG_FEATURE_FLAGS must be defined to 1 
 *          to have flag support compiled in.@n
 *          ::POSCFG_FEATURE_FLAGWAIT must be defined to 1
 *          to have this function compiled in.@n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosFlagCreate, nosFlagSet, nosFlagGet, HZ, MS
 */
#if DOX
NANOEXT VAR_t POSCALL nosFlagWait(NOSFLAG_t flg, UINT_t timeoutticks);
#else
#define nosFlagWait(flg, to)  posFlagWait((POSFLAG_t)(flg), to)
#endif
#endif

#define NOSFLAG_MODE_GETSINGLE  POSFLAG_MODE_GETSINGLE
#define NOSFLAG_MODE_GETMASK    POSFLAG_MODE_GETMASK

#endif /* NOSCFG_FEATURE_FLAGS */
/** @} */

/** @defgroup nanotimer Timer Functions
 * @ingroup absfunc
 * A timer object is a counting variable that is counted down by the
 * system timer interrupt tick rate. If the variable reaches zero,
 * a semaphore, that is bound to the timer, will be signaled.
 * If the timer is in auto reload mode, the timer is restarted and
 * will signal the semaphore again and again, depending on the
 * period rate the timer is set to.
 * @{
 */

#if DOX!=0 || NOSCFG_FEATURE_TIMER != 0

/** Handle to a nano layer timer object. */
typedef  POSTIMER_t  NOSTIMER_t;

/**
 * Timer function.
 * Allocates a timer object. After a timer is allocated with this function,
 * it must be set up with ::nosTimerSet and than started with ::nosTimerStart.
 * @return  handle to the new timer object. NULL is returned on error.
 * @param   name      Name of the new timer object to create. If the last
 *                    character in the name is an asteriks (*), the operating
 *                    system automatically assigns the timer an unique
 *                    name (the registry feature must be enabled for this
 *                    automatism). This parameter can be NULL if the nano
 *                    layer registry feature is not used and will not be
 *                    used in future.
 * @note    ::NOSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          You must use ::nosTimerDestroy to destroy the flag object again.@n
 *          Even if the function posTimerDestroy would work also, it is
 *          required to call ::nosTimerDestroy. Only this function removes
 *          the flag object from the registry. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTimerSet, nosTimerStart, nosTimerDestroy
 */
#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0
NANOEXT NOSTIMER_t POSCALL nosTimerCreate(const char *name);
#else
#define nosTimerCreate(name)  (NOSTIMER_t) posTimerCreate()
#endif

/**
 * Timer function.
 * Sets up a timer object.
 * @param   tmr  handle to the timer object.
 * @param   sema seaphore object that shall be signaled when timer fires.
 * @param   waitticks    number of initial wait ticks. The timer fires the
 *                       first time when this ticks has been expired.
 * @param   periodticks  After the timer has fired, it is reloaded with
 *                       this value, and will fire again when this count
 *                       of ticks has been expired (auto reload mode).
 *                       If this value is set to zero, the timer
 *                       won't be restarted (= one shot mode).
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTimerCreate, nosTimerStart
 */
#if DOX
NANOEXT VAR_t POSCALL nosTimerSet(NOSTIMER_t tmr, NOSSEMA_t sema,
                                  UINT_t waitticks, UINT_t periodticks);
#else
#define nosTimerSet(t, s, w, p) \
          posTimerSet((POSTIMER_t)(t), (POSSEMA_t)(s), w, p)
#endif

/**
 * Timer function.
 * Starts a timer. The timer will fire first time when the
 * waitticks counter has been reached zero. If the periodticks
 * were set, the timer will be reloaded with this value.
 * @param   tmr  handle to the timer object.
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTimerStop, nosTimerFired
 */
#if DOX
NANOEXT VAR_t POSCALL nosTimerStart(NOSTIMER_t tmr);
#else
#define nosTimerStart(tmr)  posTimerStart((POSTIMER_t)(tmr))
#endif

/**
 * Timer function.
 * Stops a timer. The timer will no more fire. The timer
 * can be reenabled with ::nosTimerStart.
 * @param   tmr   handle to the timer object.
 * @return  zero on success.
 * @note    ::NOSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTimerStart, nosTimerDestroy
 */
#if DOX
NANOEXT VAR_t POSCALL nosTimerStop(NOSTIMER_t tmr);
#else
#define nosTimerStop(tmr)  posTimerStop((POSTIMER_t)(tmr))
#endif

#if DOX!=0 || POSCFG_FEATURE_TIMERDESTROY != 0
/**
 * Timer function.
 * Deletes a timer object and free its resources.
 * @param   tmr  handle to the timer object.
 * @note    ::NOSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          ::POSCFG_FEATURE_TIMERDESTROY must be defined to 1
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTimerCreate
 */
#if DOX!=0 || NOSCFG_FEATURE_REGISTRY != 0
NANOEXT void POSCALL nosTimerDestroy(NOSTIMER_t tmr);
#else
#define nosTimerDestroy(tmr)  posTimerDestroy((POSTIMER_t)(tmr))
#endif
#endif

#if DOX!=0 || POSCFG_FEATURE_TIMERFIRED != 0
/**
 * Timer function.
 * The function is used to test if a timer has fired.
 * @param   tmr  handle to the timer object.
 * @return  1 when the timer has fired, otherwise 0.
 *          A negative value is returned on error.
 * @note    ::NOSCFG_FEATURE_TIMER must be defined to 1 
 *          to have timer support compiled in. @n
 *          ::POSCFG_FEATURE_TIMERFIRED must be defined to 1
 *          to have this function compiled in. @n
 *          Dependent of your configuration, this function can
 *          be defined as macro to decrease code size.
 * @sa      nosTimerCreate, nosTimerSet, nosTimerStart
 */
#if DOX
NANOEXT VAR_t POSCALL nosTimerFired(NOSTIMER_t tmr);
#else
#define nosTimerFired(tmr)  posTimerFired((POSTIMER_t)(tmr))
#endif
#endif

#endif /* NOSCFG_FEATURE_TIMER */
/** @} */

#undef NANOEXT
/** @} */



/*---------------------------------------------------------------------------
 *  INITIALIZATION
 *-------------------------------------------------------------------------*/

/** @defgroup nanoinit Nano Layer Initialization
 * @ingroup userapin
 * @{
 */

#ifdef _N_CORE_C
#define NANOEXT
#else
#define NANOEXT extern
#endif

/**
 * Operating System Initialization.
 * This function initializes the operating system (pico layer and nano layer)
 * and starts the first tasks: the idle task and the first user task.
 * Note: The nano layer requires dynamic memory management.
 * If ::NOSCFG_MEM_MANAGER_TYPE is set to 1 (=use internal memory manager),
 * it is required to set the variables ::__heap_start and
 * ::__heap_end to valid values before this function is called.
 *
 * @param   firstfunc   pointer to the first task function that
 *                      will run in the multitasking environment.
 * @param   funcarg     optional argument passed to the first task.
 * @param   priority    task priority. Must be in the range
 *                      0 .. ::POSCFG_MAX_PRIO_LEVEL - 1.
 *                      The higher the number, the higher the priority.
 * @param   taskStackSize  size of the stack memory for the first task.
 * @param   idleStackSize  size of the stack memory for the idle task.
 * @note    This function replaces the function ::posInit if the nano
 *          layer is enabled and linked to the destination application.
 */
NANOEXT void POSCALL nosInit(POSTASKFUNC_t firstfunc,
                             void *funcarg, VAR_t priority,
                             UINT_t taskStackSize, UINT_t idleStackSize);

#ifndef _N_CORE_C
#ifndef _POSCORE_C
#define posInit _Please_use_nosInit_instead_of_posInit_
#endif
#endif
#undef NANOEXT
/** @} */

#endif /* _POS_NANO_H */

