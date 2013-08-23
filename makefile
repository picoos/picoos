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
#  $Id: makefile,v 1.4 2006/03/14 18:45:43 dkuschel Exp $


# Set root path and include base make file
RELROOT =
include $(RELROOT)make/common.mak

# --------------------------------------------------------------------------

# Set target file name
TARGET = picoos

# Set pico]OS core source files
FILES_CORE = \
    $(DIR_SRC)/pico/picoos.c

# Add generic "findbit" function if requested
ifeq '$(strip $(GENERIC_FINDBIT))' '1'
FILES_CORE += $(DIR_SRC)/pico/fbit_gen.c
endif

# Add nano layer if configured so
ifeq '$(strip $(NANO))' '1'
FILES_CORE += $(wildcard $(DIR_SRC)/nano/*$(EXT_C))
endif

# Set port source files
FILES_PORT = \
	$(wildcard $(DIR_PORT)/*$(EXT_C)) \
	$(wildcard $(DIR_PORT)/*$(EXT_ASM)) \
	$(wildcard $(DIR_PORT)/boot/*$(EXT_C)) \
	$(wildcard $(DIR_PORT)/boot/*$(EXT_ASM))

# Set header files
FILES_HEADER = \
	$(wildcard $(DIR_INC)/*.h) \
	$(wildcard $(DIR_SRC)/*.h) \
	$(wildcard $(DIR_PORT)/*.h) \
	$(wildcard $(DIR_PORT)/boot/*.h) \
	$(wildcard $(DIR_PORT)/default/*.h)

# Set additional files in CPU subdirectories
ifneq '$(strip $(CPU))' ''

FILES_PORT += \
	$(wildcard $(DIR_PORT)/$(CPU)/*$(EXT_C)) \
	$(wildcard $(DIR_PORT)/$(CPU)/*$(EXT_ASM)) \
	$(wildcard $(DIR_PORT)/$(CPU)/boot/*$(EXT_C)) \
	$(wildcard $(DIR_PORT)/$(CPU)/boot/*$(EXT_ASM))

FILES_HEADER = \
	$(wildcard $(DIR_PORT)/$(CPU)/*.h) \
	$(wildcard $(DIR_PORT)/$(CPU)/boot/*.h) \
	$(wildcard $(DIR_PORT)/$(CPU)/default/*.h)

endif

# ---------------------------------------------------------------------------

# Build sources list
SRC_TXT = $(FILES_CORE) $(FILES_PORT)

# Set dependencies
COMMONDEP += $(FILES_HEADER)

# ---------------------------------------------------------------------------

# Build the picoos library
include $(MAKE_LIB)

# ---------------------------------------------------------------------------

# Build the doxygen documentation

$(RELROOT)doc:
	$(MKDIR) $@
$(RELROOT)doc/html: $(RELROOT)doc
	$(MKDIR) $@

.PHONY: docu

docu: $(RELROOT)doc/html $(RELROOT)make/doxygen.cfg
	doxygen make/doxygen.cfg
