#  Copyright (c) 2004-2012, Dennis Kuschel / Swen Moczarski
#  All rights reserved. 
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#   1. Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   3. The name of the author may not be used to endorse or promote
#      products derived from this software without specific prior written
#      permission. 
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
#  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
#  INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
#  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGE.


#  This file is originally from the pico]OS realtime operating system
#  (http://picoos.sourceforge.net).
#
#  $Id: port.mak,v 1.3 2006/03/11 13:06:22 dkuschel Exp $


# Set default compiler.
# Possible compilers are GCC (GNU C) and MW (MetaWare HighC/C++).
ifeq '$(strip $(COMPILER))' ''
COMPILER = GCC
endif
export COMPILER

# Set to 1 to include generic pico]OS "findbit" function
GENERIC_FINDBIT = 0

# Define extensions
EXT_C   = .c
EXT_ASM = .s
EXT_INT = .i
EXT_OBJ = .o
EXT_LIB = .a
EXT_OUT = .out


#----------------------------------------------------------------------------
#  METAWARE HIGH-C/C++
#
ifeq '$(COMPILER)' 'MW'

# Define tools: compiler, assembler, archiver, linker
CC = hcppc
AS = asppc
AR = arppc
LD = ldppc

# Define to 1 if CC outputs an assembly file
CC2ASM = 0

# Define to 1 if assembler code must be preprocessed by the compiler
A2C2A  = 1

# Define general options
OPT_CC_INC   = -I
OPT_CC_DEF   = -D
OPT_AS_INC   = -I
OPT_AS_DEF   = -D
OPT_AR_ADD   =
OPT_LD_SEP   =
OPT_LD_PFOBJ =
OPT_LD_PFLIB =
OPT_LD_FIRST =
OPT_LD_LAST  =

# Set global defines for compiler / assembler
CDEFINES = MW
ADEFINES = MW

# Set global includes
CINCLUDES = .
AINCLUDES = .

# Distinguish between build modes
ifeq '$(BUILD)' 'DEBUG'
  CFLAGS   += -O0 -g -Hon=DWARF
  AFLAGS   += -g
  CDEFINES += _DBG
  ADEFINES += _DBG
else
  CFLAGS   += -Hi -O6 -Hoff=Behaved
  CDEFINES += _REL
  ADEFINES += _REL
endif

# Define Compiler Flags
CFLAGS += -c -Hthread -HB -Hnocopyr -Hon=non_fp_varargs \
  -Hobjdir=$(DIR_OBJ) -Htmpprefix=$(DIR_OBJ) -Hppc440 \
  -Hoff=Command_line_ident -fsoft -D_ELF_EABI_ -DMW 

# Define Compiler preprocessor flags
CAFLAGS += -Hasmcpp -P \
  -Hobjdir=$(DIR_OBJ) -Htmpprefix=$(DIR_OBJ) -D_ELF_EABI_ 

# Define Assembler Flags
ASFLAGS += -c -be -tppc440 -D_ELF_EABI_ -o 

# Define Linker Flags
LDFLAGS = $(LINKERFLAGS) \
  -Bstart_addr=0x00100000 \
  -Bpage_size=0x4 -Ball_archive -e _start \
  -D $(MAPFILE) -q -o 

# Define archiver flags
ARFLAGS = -rq 


#----------------------------------------------------------------------------
#  GNU C
#
else

# Define tools: compiler, assembler, archiver, linker
CC = powerpc-eabi-gcc
AS = powerpc-eabi-gcc
AR = powerpc-eabi-ar
LD = powerpc-eabi-gcc

# Define to 1 if CC outputs an assembly file
CC2ASM = 0

# Define to 1 if assembler code must be preprocessed by the compiler
A2C2A  = 0

# Define general options
OPT_CC_INC   = -I
OPT_CC_DEF   = -D
OPT_AS_INC   = -I
OPT_AS_DEF   = -D
OPT_AR_ADD   =
OPT_LD_SEP   =
OPT_LD_PFOBJ =
OPT_LD_PFLIB =
OPT_LD_FIRST =
OPT_LD_LAST  =

# Set global defines for compiler / assembler
CDEFINES = GCC
ADEFINES = GCC

# Set global includes
CINCLUDES = .
AINCLUDES = .

# Distinguish between build modes
ifeq '$(BUILD)' 'DEBUG'
  CFLAGS   += -O0 -g
  AFLAGS   += -g
  CDEFINES += _DBG
  ADEFINES += _DBG
else
  CFLAGS   += -O3
  CDEFINES += _REL
  ADEFINES += _REL
endif

# Define Compiler Flags
CFLAGS +=  -mcpu=405 -Wall -c -o

# Define Assembler Flags
ASFLAGS += -c -mcpu=405 -Wa,-mregnames -D_ELF_EABI_ \
           -x assembler-with-cpp -o

# Define Linker Flags
LDFLAGS = $(LINKERFLAGS) -mcpu=405 -T $(MAPFILE) \
          -Wl,-Map,$(DIR_OUT)/$(TARGET).map \
          -Wl,--cref -Wl,-e,_start -o

# Define archiver flags
ARFLAGS = r 

endif

