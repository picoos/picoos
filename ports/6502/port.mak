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
#  $Id: port.mak,v 1.4 2006/10/15 09:18:47 dkuschel Exp $


# Compiler: Define target type
TG = c64

# Set to 1 to include generic pico]OS "findbit" function
GENERIC_FINDBIT = 0

# Define extensions
EXT_C   = .c
EXT_ASM = .s
EXT_OBJ = .o
EXT_LIB = .lib
EXT_OUT = .$(TG)

# Define tools: compiler, assembler, archiver, linker
ifeq '$(strip $(CC65_HOME))' ''
$(warning CC65_HOME variable not set. Make may fail compiling the sources)
CC65BIN =
else
CC65BIN = $(CC65_HOME)/bin/
endif
CC = $(CC65BIN)cc65$(EEXT)
AS = $(CC65BIN)ca65$(EEXT)
AR = $(CC65BIN)ar65$(EEXT)
LD = $(CC65BIN)ld65$(EEXT)

# Define to 1 if CC outputs an assembly file
CC2ASM = 1

# Define general options
OPT_CC_INC   = -I
OPT_CC_DEF   = -D
OPT_AS_INC   = -I
OPT_AS_DEF   = -D
OPT_AR_ADD   =
OPT_LD_SEP   = 
OPT_LD_PFOBJ = 
OPT_LD_PFLIB = 
OPT_LD_FIRST = $(TG).o
OPT_LD_LAST  = $(TG).lib

# Set global defines for compiler / assembler
CDEFINES =
ADEFINES =

# Set global includes
CINCLUDES = .
AINCLUDES = .

# Distinguish between build modes
ifeq '$(BUILD)' 'DEBUG'
  CFLAGS   += -g
  AFLAGS   += -g
  CDEFINES += _DBG
  ADEFINES += _DBG
else
  CDEFINES += _REL
  ADEFINES += _REL
endif

# Define Compiler Flags
CFLAGS += -t $(TG) -O -T -o 

# Define Assembler Flags
ASFLAGS += -t $(TG) -o 

# Define Linker Flags
LDFLAGS = -t $(TG) -Ln $(DIR_OUT)/$(TARGET).lbl -m $(DIR_OUT)/$(TARGET).map -o 
#LDFLAGS = -t $(TG) -o 

# Define archiver flags
ARFLAGS = a 
