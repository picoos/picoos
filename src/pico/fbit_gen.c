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


/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 *
 * CVS-ID $Id: fbit_gen.c,v 1.2 2006/10/15 08:51:42 dkuschel Exp $
 */


#define _FBIT_GEN_C
#include <picoos.h>


/* Possible configurations for this file
 * -------------------------------------
 *
 * You may add the defines 
 *    POSCFG_FBIT_USE_LUTABLE  and  POSCFG_FBIT_BITSHIFT
 * to your pico]OS configuration file.
 *
 * POSCFG_FBIT_USE_LUTABLE = 0:
 *  Do not use look up tables. "findbit" is implemented as a function.
 *  (This does not increase code size through tables. Also
 *  some CPUs may execute program code faster from their caches
 *  than fetching data from big lookup tables.)
 *  Note: This is the only possible setting for systems with MVAR_BITS != 8
 *
 * POSCFG_FBIT_USE_LUTABLE = 1:
 *  - When round robin scheduling is disabled, findbit is done
 *    by use of a 256 byte sized lookup table.
 *  - When round robin scheduling is enabled, findbit is implemented
 *    as a function and uses a 256 byte sized lookup table.
 *
 * POSCFG_FBIT_USE_LUTABLE = 2:
 *  This is only applicable for round robin scheduling.
 *  "findbit" is implemented as a two dimensional lookup table.
 *  This blows code size very much.
 *
 * POSCFG_FBIT_BITSHIFT = 1:
 *  Set this to 1 if your machine is able to do fast bit shifts.
 *  This is true for most of the bigger machines such as PowerPC.
 */


#ifndef POSCFG_FBIT_BITSHIFT
#if (MVAR_BITS < 32)
#define POSCFG_FBIT_BITSHIFT  0
#else
#define POSCFG_FBIT_BITSHIFT  1
#endif
#endif
#ifndef POSCFG_FBIT_USE_LUTABLE
#define POSCFG_FBIT_USE_LUTABLE  0
#endif

#if (POSCFG_FBIT_USE_LUTABLE != 0) && (MVAR_BITS != 8)
#error This file implements only lookup-tables for MVAR_BITS == 8
#endif

#if (POSCFG_FBIT_USE_LUTABLE == 1)

VAR_t const p_pos_fbittbl[256] =
{ 0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
  6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
  7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
  6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0 };

#if (POSCFG_ROUNDROBIN != 0)

/* Round Robin generic finbit() -function using a look-up table.
 */
UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset)
{
  UVAR_t bf, bit;
  bf = (bitfield << (MVAR_BITS - rrOffset)) | (bitfield >> rrOffset); 
  bit = p_pos_fbittbl[bf];
  return (bit + rrOffset) & (MVAR_BITS - 1);
}
#endif /* POSCFG_ROUNDROBIN */

#elif (POSCFG_FBIT_USE_LUTABLE == 2)

VAR_t const p_pos_fbittbl_rr[8][256] =
{
  /* 0 */
  { 0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0 },

  /* 1 */
  { 0,0,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1,
    5,5,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1,
    6,6,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1,
    5,5,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1,
    7,7,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1,
    5,5,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1,
    6,6,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1,
    5,5,1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4,1,1,2,2,1,1,3,3,1,1,2,2,1,1 },

  /* 2 */
  { 0,0,1,0,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2,
    5,5,5,5,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2,
    6,6,6,6,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2,
    5,5,5,5,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2,
    7,7,7,7,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2,
    5,5,5,5,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2,
    6,6,6,6,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2,
    5,5,5,5,2,2,2,2,3,3,3,3,2,2,2,2,4,4,4,4,2,2,2,2,3,3,3,3,2,2,2,2 },

  /* 3 */
  { 0,0,1,0,2,0,1,0,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,
    5,5,5,5,5,5,5,5,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,
    6,6,6,6,6,6,6,6,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,
    5,5,5,5,5,5,5,5,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,
    7,7,7,7,7,7,7,7,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,
    5,5,5,5,5,5,5,5,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,
    6,6,6,6,6,6,6,6,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,
    5,5,5,5,5,5,5,5,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3 },

  /* 4 */
  { 0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4 },

  /* 5 */
  { 0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5 },

  /* 6 */
  { 0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 },

  /* 7 */
  { 0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7 }
};


#else /* POSCFG_FBIT_USE_LUTABLE */
/*-------------------------------------------------------------------------*/


#if (POSCFG_FASTCODE != 0) && (MVAR_BITS <= 32)
#if (POSCFG_FBIT_BITSHIFT != 0)

/*
 * Fast generic findbit() -function for 8/16/32 bit architectures.
 * The code supports roundrobin and standard-scheduling.
 *
 * Speed:
 *   8 bit: 3 if-branches
 *  16 bit: 4 if-branches
 *  32 bit: 5 if-branches
 *
 * When roundrobin is enabled, also two shift-, one and-, one or-
 * and one addition operation are needed.
 *
 * Note:
 *  This function is the best choice for architectures that
 *  have a fast bit-shift support. This is true for most of
 *  the 16 and 32 bit architectures.
 */

#if (POSCFG_ROUNDROBIN == 0)

UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield)
{
  UVAR_t bf = bitfield;
  UVAR_t bit;

#else /* POSCFG_ROUNDROBIN */

UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset)
{
  UVAR_t bf;
  UVAR_t bit;
  
  bf = (bitfield << (MVAR_BITS - rrOffset)) | (bitfield >> rrOffset);

#endif /* POSCFG_ROUNDROBIN */

  bit = 0;

#if (MVAR_BITS > 16)
  if ((bf & 0xFFFF) == 0)
  {
    bit |= 16;
    bf >>= 16;
  }
#endif
#if (MVAR_BITS > 8)
  if ((bf & 0x00FF) == 0)
  {
    bit |= 8;
    bf >>= 8;
  }
#endif
  if ((bf & 0x000F) == 0)
  {
    bit |= 4;
    bf >>= 4;
  }
  if ((bf & 0x0003) == 0)
  {
    bit |= 2;
    bf >>= 2;
  }
  if ((bf & 0x0001) == 0)
  {
    bit |= 1;
  }

#if (POSCFG_ROUNDROBIN == 0)
  return bit;
#else /* POSCFG_ROUNDROBIN */
  return (bit + rrOffset) & (MVAR_BITS - 1);
#endif /* POSCFG_ROUNDROBIN */
}

#else /* POSCFG_FBIT_BITSHIFT */

/*
 * Fast generic findbit() -function for 8/16/32 bit architectures.
 * The code supports roundrobin and standard-scheduling.
 *
 * Speed:
 *   8 bit: 3 if-branches
 *  16 bit: 4 if-branches
 *  32 bit: 5 if-branches
 *
 * When roundrobin is enabled, also two shift-, one and-, one or-
 * and one addition operation are needed.
 *
 * Note:
 *  This function can be used for architectures that
 *  are slow in bit shifting. This function may
 *  be used for 8 or 16 bit architectures.
 */

#if (POSCFG_ROUNDROBIN == 0)

UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield)
{
  UVAR_t bf = bitfield;
  UVAR_t bit;

#else /* POSCFG_ROUNDROBIN */

UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset)
{
  UVAR_t bf;
  UVAR_t bit;
  
  bf = (bitfield << (MVAR_BITS - rrOffset)) | (bitfield >> rrOffset);

#endif /* POSCFG_ROUNDROBIN */

#if (MVAR_BITS == 32)
  if (bf & 0x0000FFFF) {
#endif
#if (MVAR_BITS >= 16)
    if (bf & 0x00FF) {
#endif
      if (bf & 0x0F) {
        if (bf & 0x03) {
          if (bf & 0x01) {
            bit = 0;
          } else {
            bit = 1;
          }
        } else {
          if (bf & 0x04) {
            bit = 2;
          } else {
            bit = 3;
          }
        }
      } else {
        if (bf & 0x30) {
          if (bf & 0x10) {
            bit = 4;
          } else {
            bit = 5;
          }
        } else {
          if (bf & 0x40) {
            bit = 6;
          } else {
            bit = 7;
          }
        }
      }
#if (MVAR_BITS >= 16)
    } else {
      if (bf & 0x0F00) {
        if (bf & 0x0300) {
          if (bf & 0x0100) {
            bit = 8;
          } else {
            bit = 9;
          }
        } else {
          if (bf & 0x0400) {
            bit = 10;
          } else {
            bit = 11;
          }
        }
      } else {
        if (bf & 0x3000) {
          if (bf & 0x1000) {
            bit = 12;
          } else {
            bit = 13;
          }
        } else {
          if (bf & 0x4000) {
            bit = 14;
          } else {
            bit = 15;
          }
        }
      }
    }  
#endif
#if (MVAR_BITS == 32)
  } else { 
    if (bf & 0x00FF0000) {
      if (bf & 0x000F0000) {
        if (bf & 0x00030000) {
          if (bf & 0x00010000) {
            bit = 16;
          } else {
            bit = 17;
          }
        } else {
          if (bf & 0x00040000) {
            bit = 18;
          } else {
            bit = 19;
          }
        }
      } else {
        if (bf & 0x00300000) {
          if (bf & 0x00100000) {
            bit = 20;
          } else {
            bit = 21;
          }
        } else {
          if (bf & 0x00400000) {
            bit = 22;
          } else {
            bit = 23;
          }
        }
      }
    } else {
      if (bf & 0x0F000000) {
        if (bf & 0x03000000) {
          if (bf & 0x01000000) {
            bit = 24;
          } else {
            bit = 25;
          }
        } else {
          if (bf & 0x04000000) {
            bit = 26;
          } else {
            bit = 27;
          }
        }
      } else {
        if (bf & 0x30000000) {
          if (bf & 0x10000000) {
            bit = 28;
          } else {
            bit = 29;
          }
        } else {
          if (bf & 0x40000000) {
            bit = 30;
          } else {
            bit = 31;
          }
        }
      }
    }  
  }
#endif

#if (POSCFG_ROUNDROBIN == 0)
  return bit;
#else /* POSCFG_ROUNDROBIN */
  return (bit + rrOffset) & (MVAR_BITS - 1);
#endif /* POSCFG_ROUNDROBIN */
}

#endif /* POSCFG_FBIT_BITSHIFT */
#else  /* FASTCODE */
#if (POSCFG_ROUNDROBIN == 0)

/*
 * Slow but small generic findbit() -function for all architectures.
 * The code supports standard-scheduling.
 *
 */

UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield)
{
  UVAR_t bf = bitfield;
  UVAR_t bit;
  
  for (bit = 0; bit < MVAR_BITS; bit++)
  {
    if (bf & 1)
      break;
    bf >>= 1;
  }
 
  return bit;
}

#else /* POSCFG_ROUNDROBIN */

/*
 * Slow but small generic findbit() -function for all architectures.
 * The code supports roundrobin-scheduling.
 *
 */

UVAR_t POSCALL p_pos_findbit(const UVAR_t bitfield, UVAR_t rrOffset)
{
  UVAR_t bf;
  UVAR_t bit;
  
  bf = (bitfield << (MVAR_BITS - rrOffset)) | (bitfield >> rrOffset);
  
  for (bit = 0; bit < MVAR_BITS; bit++)
  {
    if (bf & 1)
      break;
    bf >>= 1;
  }
 
  return (bit + rrOffset) & (MVAR_BITS - 1);
}

#endif  /* ROUNDROBIN */
#endif  /* FASTCODE */
#endif  /* POSCFG_FBIT_USE_LUTABLE */
