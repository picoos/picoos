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
#  $Id: common.mak,v 1.7 2006/10/15 08:50:51 dkuschel Exp $


# Include configuration
-include $(RELROOT)config.mak

# Set default target
default:  all

# Listing of phony targets.
.PHONY: clean env

# Save the current make target
MTARGET = $(firstword $(MAKECMDGOALS) all)

# Define shell commands
# First: Determine if we are running under DOS/Windows or Unix
DOS =
ifeq '$(strip $(basename $(notdir $(SHELL))))' 'command'
DOS = 1
else
ifeq '$(strip $(basename $(notdir $(SHELL))))' 'cmd'
DOS = 2
endif
endif
ifeq '$(strip $(basename $(notdir $(MAKESHELL))))' 'command'
DOS += 1
else
ifeq '$(strip $(basename $(notdir $(MAKESHELL))))' 'cmd'
DOS += 2
endif
endif
ifeq '$(suffix $(SHELL))' '.exe'
DOS += 3
endif
ifeq '$(suffix $(SHELL))' '.com'
DOS += 3
endif

ifeq '$(strip $(DOS))' ''
# Set executable extension
EEXT = .exe
# Set host
COMPILEHOST = DOS
# Delete path
SHCMDPATH =
# Generate macro for Unix style paths
adjpath = $(subst \,/,$(1))
# Get current path
CURRENTDIR = $(shell pwd)
else
# Set host
COMPILEHOST = LINUX
# Set executable extension
EEXT =
# Set path to tools
SHCMDPATH = $(RELROOT)make/tools/
# Generate macro for DOS style paths
adjpath = $(subst /,\,$(1))
# Get current path
CURRENTDIR = $(subst \,/,$(shell cd))
ifeq '$(strip $(CURRENTDIR))' ''
CURRENTDIR := $(subst \,/,$(shell cmd /Ccd))
ifeq '$(strip $(CURRENTDIR))' ''
$(warning Failed to determine the current directory!)
endif
endif
ifneq '$(strip $(firstword $(CURRENTDIR)))' '$(strip $(CURRENTDIR))'
$(warning Current directory path contains spaces! ($(CURRENTDIR)))
endif
endif

# Define commands. DOS users will use the commands in the tools/ directory.
MKDIR  = $(SHCMDPATH)mkdir$(EEXT)
REMOVE = $(SHCMDPATH)rm$(EEXT) -f
RMDIR  = $(SHCMDPATH)rm$(EEXT) -f
ifeq '$(strip $(DOS))' ''
ECHO   = echo
else
# note: not all versions of make support DOS echo
ECHO   = @echo
endif

# ---------------------------------------------------------------------------

# Define Port Name
ifeq '$(strip $(PORT))' ''
$(warning *** !!!)
$(error !!! You must specify the name\
 of the architecture port, e.g. PORT=x86dos)
PORT =
BUILD = DEBUG
all:
clean:
else
# Define Build Mode
ifeq '$(strip $(BUILD))' ''
$(warning Build mode not specified. Now using BUILD=DEBUG)
BUILD = DEBUG
endif
endif

# Set build mode extension
ifeq '$(BUILD)' 'DEBUG'
MODEEXT = deb
else
ifeq '$(BUILD)' 'RELEASE'
MODEEXT = rel
else
$(error Unknown build mode: $(BUILD))
endif
endif

export BUILD
export PORT
export CPU
export EEXT
export COMPILEHOST

# Define paths
DIR_OUTB = $(RELROOT)out
DIR_OUTP = $(DIR_OUTB)/$(PORT)
DIR_OUT  = $(DIR_OUTP)/$(MODEEXT)
DIR_LIBB = $(RELROOT)lib
DIR_LIBP = $(DIR_LIBB)/$(PORT)
DIR_LIB  = $(DIR_LIBP)/$(MODEEXT)
DIR_OBJB = $(RELROOT)obj
DIR_OBJP = $(DIR_OBJB)/$(PORT)
DIR_OBJ  = $(DIR_OBJP)/$(MODEEXT)
DIR_SRC  = $(RELROOT)src
DIR_INC  = $(RELROOT)inc
DIR_PORT = $(RELROOT)ports/$(PORT)

ifneq '$(strip DIR_CONFIG)' ''
export DIR_CONFIG
endif

# ---------------------------------------------------------------------------

# Include port specific make file.
# This will define the compiler environment to use.
ifneq '$(PORT)' ''
include $(DIR_PORT)/port.mak
endif

# Set defaults
ifeq '$(strip $(EXT_C))' ''
EXT_C := .c
endif
ifeq '$(strip $(EXT_ASM))' ''
EXT_ASM := .s
endif
ifeq '$(strip $(EXT_INT))' ''
EXT_INT := .i
endif
ifeq '$(strip $(EXT_OBJ))' ''
EXT_OBJ := .o
endif
ifeq '$(strip $(EXT_LIB))' ''
EXT_LIB := .a
endif

# Set pico]OS package flag
CDEFINES += _POSPACK
ADEFINES += _POSPACK

# ---------------------------------------------------------------------------

# Set make file names
MAKE_LIB = $(RELROOT)make/lib.mak
MAKE_OUT = $(RELROOT)make/out.mak
MAKE_CPL = $(RELROOT)make/compile.mak

# ---------------------------------------------------------------------------

# Tool function
env:
	$(ECHO) MAKESHELL = $(MAKESHELL)
	$(ECHO) SHELL  = $(SHELL)
	$(ECHO) HEADER = $(FILES_HEADER)
	$(ECHO) SRC    = $(SRC)
	$(ECHO) OBJ    = $(OBJ)
	$(ECHO) TARGETLIB = $(TARGETLIB)
	$(ECHO) DEFSRCDIR = $(DEFSRCDIR)
	$(ECHO) DIR_SRC1 = $(DIR_SRC1)
	$(ECHO) DIR_SRC2 = $(DIR_SRC2)
	$(ECHO) DIR_SRC3 = $(DIR_SRC3)
	$(ECHO) DIR_SRC4 = $(DIR_SRC4)
