#  Copyright (c) 2004-2005, Dennis Kuschel / Swen Moczarski
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
#  $Id: makefile,v 1.4 2005/02/01 22:20:40 dkuschel Exp $


# The default port is x86w32
ifeq '$(strip $(PORT))' ''
PORT = x86w32
endif
export PORT

SRCDIR = examples
export DIR_USRINC = $(SRCDIR)
ifneq '$(strip $(OLIBNAME))' ''
OLIBVAR = OLIBNAME=$(OLIBNAME)
endif

ifeq '$(strip $(SOURCEFILE))' ''

# Set root path
RELROOT = ../

include $(RELROOT)make/common.mak

MAKECMD = -$(MAKE) -f $(SRCDIR)/makefile -C .. SOURCEFILE=$(SRCDIR)/
MAKECLCMD = -$(MAKE) clean -f $(SRCDIR)/makefile -C .. SOURCEFILE=$(SRCDIR)/

default: all

all:
	$(MAKECLCMD)test.c NANO=0
	$(MAKECMD)test.c NANO=0
	$(MAKECMD)ex_task1.c NANO=0
	$(MAKECMD)ex_task2.c NANO=0
	$(MAKECMD)ex_task3.c NANO=0
	$(MAKECLCMD)ex_bhalf.c NANO=1
	$(MAKECMD)ex_bhalf.c
	$(MAKECMD)ex_flag1.c
	$(MAKECMD)ex_flag2.c
	$(MAKECMD)ex_lists.c
	$(MAKECMD)ex_mesg1.c
	$(MAKECMD)ex_mesg2.c
	$(MAKECMD)ex_mutx1.c
	$(MAKECMD)ex_mutx2.c
	$(MAKECMD)ex_sema1.c
	$(MAKECMD)ex_sema2.c
	$(MAKECMD)ex_sema3.c
	$(MAKECMD)ex_sema4.c
	$(MAKECMD)ex_sint1.c
	$(MAKECMD)ex_task4.c
	$(MAKECMD)ex_timr1.c
	$(MAKECMD)ex_timr2.c

clean:
	$(MAKECLCMD)test.c NANO=0
	$(MAKECLCMD)ex_task1.c NANO=0
	$(MAKECLCMD)ex_task2.c NANO=0
	$(MAKECLCMD)ex_task3.c NANO=0
	$(MAKECLCMD)ex_bhalf.c
	$(MAKECLCMD)ex_flag1.c
	$(MAKECLCMD)ex_flag2.c
	$(MAKECLCMD)ex_lists.c
	$(MAKECLCMD)ex_mesg1.c
	$(MAKECLCMD)ex_mesg2.c
	$(MAKECLCMD)ex_mutx1.c
	$(MAKECLCMD)ex_mutx2.c
	$(MAKECLCMD)ex_sema1.c
	$(MAKECLCMD)ex_sema2.c
	$(MAKECLCMD)ex_sema3.c
	$(MAKECLCMD)ex_sema4.c
	$(MAKECLCMD)ex_sint1.c
	$(MAKECLCMD)ex_task4.c
	$(MAKECLCMD)ex_timr1.c
	$(MAKECLCMD)ex_timr2.c

else

# --------------------------------------------------------------------------

# Set root path
RELROOT = ./

# Build mode
BUILD = DEBUG

# Include nano layer if not other specified
ifeq '$(strip $(NANO))' ''
NANO = 1
endif

# Include base make file
include $(RELROOT)make/common.mak

# --------------------------------------------------------------------------

# Set target file name
TARGET = $(basename $(notdir $(SOURCEFILE)))

# Set source files
SRC_TXT = $(SOURCEFILE)
SRC_OBJ =
SRC_LIB =

# Set the directory that contains the configuration header files
#DIR_CONFIG =

# ---------------------------------------------------------------------------

# Build an executable
include $(MAKE_OUT)

endif
