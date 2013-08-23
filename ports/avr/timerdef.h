/*
 *  Copyright (c) 2004, Swen Moczarski.
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


/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: timerdef.h,v 1.7 2008/09/01 20:14:37 smocz Exp $
 */

#ifndef TIMERDEF_H
#define TIMERDEF_H 1

/**
 * In this file, the configuration for the timer will be defined.
 *
 * To support a easy way to adapt the pico]OS AVR Port on other
 * cpu types, the configuration for the timer can be done with
 * some defines.
 * The timer will be used as counter.
 *
 * TIMER_CONFIG_REG:
 *      The configuraion register of the timer.
 * TIMER_CONFIG_VALUE:
 *      The value for the configuraion register.
 *
 * TIMER_COUNTER_REG:
 *      The register for the counter of the timer.
 * TIMER_COUNTER_VALUE
 *      The value, which will be reloaded in the counter register.
 *
 * TIMER_INTERRUPT_REG
 *      The interrupt configuration register for the timer.
 * TIMER_INTERRUPT_ENABLE_BIT
 *      The bit to enable the interrupt for the timer.
 *
 * For calculating the value for TIMER_COUNTER_VALUE, two external
 * defines from "port.h" are needed:
 *      HZ:             The scheduling rate
 *      CRYSTAL_CLOCK:  The clock of the crystal
 *
 */


// defined(__AVR_ATmega328P__)
// defined(__AVR_ATmega406__) : register name?
#if	defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega164P__) || \
    defined(__AVR_ATmega162__) || \
    defined(__AVR_ATmega32__) || \
	defined(__AVR_ATmega323__) || \
	defined(__AVR_ATmega48__) || \
	defined(__AVR_ATmega640__) || \
	defined(__AVR_ATmega128__) || \
	defined(__AVR_ATmega1280__) || \
	defined(__AVR_ATmega1281__) || \
	defined(__AVR_ATmega2560__) || \
	defined(__AVR_ATmega2561__) || \
	defined(__AVR_ATmega324P__) || \
	defined(__AVR_ATmega325__) || \
	defined(__AVR_ATmega325P__) || \
	defined(__AVR_ATmega3250__) || \
	defined(__AVR_ATmega3250P__) || \
	defined(__AVR_ATmega329__) || \
	defined(__AVR_ATmega329P__) || \
	defined(__AVR_ATmega3290__) || \
	defined(__AVR_ATmega3290P__) || \
	defined(__AVR_ATmega64__) || \
	defined(__AVR_ATmega640__) || \
	defined(__AVR_ATmega644__) || \
	defined(__AVR_ATmega644P__) || \
	defined(__AVR_ATmega645__) || \
	defined(__AVR_ATmega6450__) || \
	defined(__AVR_ATmega649__) || \
	defined(__AVR_ATmega6490__)

/**
 * The flags for the prescaler in the TCCR1B register.
 *
 *  CS12  CS11  CS10            prescaler value
 *    0     0     1    (0x01)   CRYSTAL_CLOCK / 1
 *    0     1     0    (0x02)   CRYSTAL_CLOCK / 8
 *    0     1     1    (0x03)   CRYSTAL_CLOCK / 64
 *    1     0     0    (0x04)   CRYSTAL_CLOCK / 256
 *    1     0     1    (0x05)   CRYSTAL_CLOCK / 1024
 *
 * This value defines the possible range for the timer tick (HZ).
 */

// set WGM12 for Clear Timer on Compare match (CTC) mode
#  define TIMER_CONFIG_REG             TCCR1B

#  define TIMER_CONFIG_VALUE           _BV(WGM12) | 0x04

// the resulting value for the presacler is 256, see TIMER_CONFIG_VALUE
#  define TIMER_COUNTER_REG            OCR1A
#  define TIMER_COUNTER_VALUE          ((CRYSTAL_CLOCK / 256) / HZ)

#if defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega164P__) || \
    defined(__AVR_ATmega640__) || \
	defined(__AVR_ATmega1280__) || \
	defined(__AVR_ATmega1281__) || \
	defined(__AVR_ATmega2560__) || \
	defined(__AVR_ATmega2561__) || \
	defined(__AVR_ATmega324P__) || \
	defined(__AVR_ATmega325__) || \
	defined(__AVR_ATmega325P__) || \
	defined(__AVR_ATmega3250__) || \
	defined(__AVR_ATmega3250P__) || \
	defined(__AVR_ATmega329__) || \
	defined(__AVR_ATmega329P__) || \
	defined(__AVR_ATmega3290__) || \
	defined(__AVR_ATmega3290P__) || \
	defined(__AVR_ATmega640__) || \
	defined(__AVR_ATmega644__) || \
	defined(__AVR_ATmega644P__) || \
	defined(__AVR_ATmega645__) || \
	defined(__AVR_ATmega6450__) || \
	defined(__AVR_ATmega649__) || \
	defined(__AVR_ATmega6490__)

#  define TIMER_INTERRUPT_REG          TIMSK1
#else
#  define TIMER_INTERRUPT_REG          TIMSK
#endif

#  define TIMER_INTERRUPT_ENABLE_BIT   OCIE1A

#if	defined(__AVR_ATmega328P__)
#define TIMER_INTERRUPT_VECTOR TIMER1_COMPA_vect
#else
#define TIMER_INTERRUPT_VECTOR SIG_OUTPUT_COMPARE1A
#endif

#else

#  error no timer configuration for this MCU type

#endif


#endif
