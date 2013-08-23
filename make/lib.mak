#  Copyright (c) 2004, Dennis Kuschel / Swen Moczarski
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
#  $Id: lib.mak,v 1.4 2006/03/23 21:36:51 dkuschel Exp $


# Build target: generate library

ifeq '$(strip $(MAKE_LIB))' ''
($error common.mak not included)
endif

ifneq '$(strip $(OLIBNAME))' ''
TARGET := $(OLIBNAME)
endif

ifeq '$(strip $(TARGET))' ''
($error TARGET library name not set)
endif

# ---------------------------------------------------------------------------

# Set source files
SRC := $(SRC_TXT)

# Set common dependencies
COMMONDEP += $(SRC_HDR)

# Clear object file list
OBJ :=

# Export variables
ifneq '$(strip $(DIR_OUTPUT))' ''
export DIR_OUTPUT
endif

# Compile source files
include $(MAKE_CPL)

# ---------------------------------------------------------------------------

TARGETLIB = $(DIR_LIB)/$(TARGET)$(EXT_LIB)
AOBJS = $(addprefix $(OPT_AR_ADD),$(OBJ))

$(TARGETLIB): $(OBJ) $(COMMONDEP) $(SRC_OBJ) $(DIR_LIB)
	$(AR) $(ARFLAGS)$(call adjpath,$@) $(call adjpath,$(AOBJS) $(SRC_OBJ))

# ---------------------------------------------------------------------------

ifneq '$(PORT)' ''

# Set the library as default target.
all: $(TARGETLIB)

# Target: clean library.
clean: $(CLEAN_USER)
	-$(REMOVE) $(TARGETLIB)
	-$(REMOVE) $(OBJ)

endif
