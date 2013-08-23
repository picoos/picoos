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
#  $Id: compile.mak,v 1.9 2006/04/16 08:49:02 dkuschel Exp $


# Compile files

ifeq '$(strip $(MAKE_CPL))' ''
($error common.mak not included)
endif

# Ensure the port variable is set.
ifneq '$(PORT)' ''

# Set alternative binary output directory
ifeq '$(strip $(RTOS_OUTPUTDIR))' ''
ifneq '$(strip $(DIR_OUTPUT))' ''
DIR_OUTB := $(DIR_OUTPUT)
DIR_OUTP := $(DIR_OUTB)/$(PORT)-$(MODEEXT)
DIR_OUT  := $(DIR_OUTP)/out
DIR_LIBB := $(DIR_OUTPUT)
DIR_LIBP := $(DIR_LIBB)/$(PORT)-$(MODEEXT)
DIR_LIB  := $(DIR_LIBP)/lib
DIR_OBJB := $(DIR_OUTPUT)
DIR_OBJP := $(DIR_OBJB)/$(PORT)-$(MODEEXT)
DIR_OBJ  := $(DIR_OBJP)/obj
endif
ifneq '$(strip $(OLIBNAME))' ''
DIR_OBJT := $(DIR_OBJ)
DIR_OBJ  := $(DIR_OBJ)/$(OLIBNAME)
else
DIR_OBJT :=
endif
else
# user provides a custom RTOS_OUTPUTDIR
DIR_OUTB :=
DIR_OUTP :=
DIR_OUT  := $(RTOS_OUTPUTDIR)
DIR_LIBB :=
DIR_LIBP :=
DIR_LIB  := $(RTOS_OUTPUTDIR)
DIR_OBJB :=
ifneq '$(strip $(RTOS_TEMPDIR))' ''
DIR_OBJP :=
DIR_OBJ  := $(RTOS_TEMPDIR)
else
DIR_OBJP := $(RTOS_OUTPUTDIR)
DIR_OBJ  := $(RTOS_OUTPUTDIR)/obj
endif
endif

# Set default configuration directory
ifeq '$(strip $(DIR_CONFIG))' ''
ifeq '$(strip $(CPU))' ''
DCFGDEF = $(DIR_PORT)/default
else
DCFGDEF = $(DIR_PORT)/$(CPU)/default
endif
endif

# Define all object files.
OBJC = $(filter-out $(SRC),$(SRC:$(EXT_C)=$(EXT_OBJ)))
OBJA = $(filter-out $(SRC),$(SRC:$(EXT_ASM)=$(EXT_OBJ)))
OBJ += $(addprefix $(DIR_OBJ)/,$(notdir $(OBJC) $(OBJA)))

# Define source directories. Strip them from the SRC variable.
SRCDIRS  = $(sort $(dir $(SRC)))
DIR_SRC1 = $(strip $(word 1,$(SRCDIRS)))
DIR_SRC2 = $(strip $(word 2,$(SRCDIRS)))
DIR_SRC3 = $(strip $(word 3,$(SRCDIRS)))
DIR_SRC4 = $(strip $(word 4,$(SRCDIRS)))
DIR_SRC5 = $(strip $(word 5,$(SRCDIRS)))
DIR_SRC6 = $(strip $(word 6,$(SRCDIRS)))
DIR_SRC7 = $(strip $(word 7,$(SRCDIRS)))
DIR_SRC8 = $(strip $(word 8,$(SRCDIRS)))

# define something for the nano layer
ifeq '$(strip $(NANO))' '1'
export NANO
CDEFINES += POSNANO
ADEFINES += POSNANO
endif

# Place include/define options here
CINCLUDES += $(DIR_INC) $(DCFGDEF) $(DIR_CONFIG) $(DIR_PORT) $(DIR_USRINC)
AINCLUDES += $(DIR_INC) $(DCFGDEF) $(DIR_CONFIG) $(DIR_PORT) $(DIR_USRINC)
ifneq '$(strip $(CPU))' ''
CINCLUDES += $(DIR_PORT)/$(CPU)
AINCLUDES += $(DIR_PORT)/$(CPU)
endif

TCINCL = . $(filter-out .,$(CINCLUDES))
TAINCL = . $(filter-out .,$(AINCLUDES))
CINCS = $(addprefix $(OPT_CC_INC),$(call adjpath,$(TCINCL)))
CDEFS = $(addprefix $(OPT_CC_DEF),$(CDEFINES))
AINCS = $(addprefix $(OPT_AS_INC),$(call adjpath,$(TAINCL)))
ADEFS = $(addprefix $(OPT_AS_DEF),$(ADEFINES))

# Set config header file dependencies
ifneq '$(strip $(DIR_CONFIG))' ''
COMMONDEP += $(wildcard $(DIR_CONFIG)/poscfg.h) \
             $(wildcard $(DIR_CONFIG)/noscfg.h)
else
COMMONDEP += $(wildcard $(DCFGDEF)/*.h)
endif

# ---------------------------------------------------------------------------

# Make subdirectories
ifneq '$(strip $(DIR_OUTPUT))' ''
$(DIR_OUTPUT):
	-$(MKDIR) $@
endif
ifneq '$(strip $(DIR_OBJB))' ''
ifneq '$(DIR_OBJB)' '$(DIR_OUTPUT)'
$(DIR_OBJB): $(DIR_OUTPUT)
	-$(MKDIR) $@
endif
endif
ifneq '$(strip $(DIR_OBJP))' ''
$(DIR_OBJP): $(DIR_OBJB)
	-$(MKDIR) $@
endif
ifneq '$(strip $(DIR_OBJT))' ''
$(DIR_OBJT): $(DIR_OBJP)
	-$(MKDIR) $@
endif
$(DIR_OBJ): $(DIR_OBJT) $(DIR_OBJP)
	-$(MKDIR) $@
#
ifneq '$(DIR_LIB)' '$(DIR_OBJ)'
ifneq '$(DIR_LIBB)' '$(DIR_OUTPUT)'
ifneq '$(strip $(DIR_LIBB))' ''
$(DIR_LIBB): $(DIR_OUTPUT)
	-$(MKDIR) $@
endif
endif
ifneq '$(DIR_LIBP)' '$(DIR_OBJP)'
ifneq '$(strip $(DIR_LIBP))' ''
$(DIR_LIBP): $(DIR_LIBB)
	-$(MKDIR) $@
endif
endif
ifneq '$(DIR_LIB)' '$(DIR_OBJP)'
$(DIR_LIB): $(DIR_LIBP)
	-$(MKDIR) $@
endif
endif
#
ifneq '$(DIR_OUT)' '$(DIR_LIB)'
ifneq '$(DIR_OUTB)' '$(DIR_OUTPUT)'
ifneq '$(strip $(DIR_OUTB))' ''
$(DIR_OUTB): $(DIR_OUTPUT)
	-$(MKDIR) $@
endif
endif
ifneq '$(DIR_OUTP)' '$(DIR_OBJP)'
ifneq '$(strip $(DIR_OUTP))' ''
$(DIR_OUTP): $(DIR_OUTB)
	-$(MKDIR) $@
endif
endif
$(DIR_OUT): $(DIR_OUTP)
	-$(MKDIR) $@
endif

# ---------------------------------------------------------------------------

# Build a macro that is used to generate the correct C-flags
ifneq '$(strip $(OPT_CC_ODIR))' ''
ifneq '$(strip $(OPT_CC_OFILE))' ''
make_cflags = $(CFLAGS) $(OPT_CC_ODIR)$(call adjpath,$(DIR_OBJ)) $(OPT_CC_OFILE)$(call adjpath,$(1))
else
make_cflags = $(CFLAGS) $(OPT_CC_ODIR)$(call adjpath,$(DIR_OBJ))
endif
else
ifneq '$(strip $(OPT_CC_OFILE))' ''
make_cflags = $(CFLAGS) $(OPT_CC_OFILE)$(call adjpath,$(1))
else
make_cflags = $(CFLAGS)$(call adjpath,$(1))
endif
endif

# Build a macro that is used to generate the correct Assembler-flags
ifneq '$(strip $(OPT_AS_ODIR))' ''
ifneq '$(strip $(OPT_AS_OFILE))' ''
make_asflags = $(ASFLAGS) $(OPT_AS_ODIR)$(call adjpath,$(DIR_OBJ)) $(OPT_AS_OFILE)$(call adjpath,$(1))
else
make_asflags = $(ASFLAGS) $(OPT_AS_ODIR)$(call adjpath,$(DIR_OBJ))
endif
else
ifneq '$(strip $(OPT_AS_OFILE))' ''
make_asflags = $(ASFLAGS) $(OPT_AS_OFILE)$(call adjpath,$(1))
else
make_asflags = $(ASFLAGS)$(call adjpath,$(1))
endif
endif

ifeq '$(strip $(CC2ASM))' '1'

# Compile: create intermediate files from C source files, and then
# assemble: create object files from intermediate files
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC1)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC2)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC3)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC4)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC5)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC6)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC7)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
$(DIR_OBJ)/%$(EXT_OBJ): $(DIR_SRC8)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**)) $(CINCS) $(CDEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))
	-$(REMOVE) $(call adjpath,$(subst $(EXT_OBJ)**,$(EXT_INT),$@**))

else

# Compile: create object files from C source files
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC1)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC2)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC3)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC4)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC5)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC6)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC7)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC8)%$(EXT_C) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(call make_cflags,$@) $(CINCS) $(CDEFS) $(call adjpath,$<)

endif

ifeq '$(strip $(A2C2A))' '1'

# Assemble: create object files from assembler source files,
# but preprocess the assembler files with the C-compiler preprocessor.
# (This code matches the MetaWare HighC/C++ compiler)
CAINCS = $(call adjpath,$(addprefix $(OPT_CC_INC),$(AINCLUDES)))
CADEFS = $(addprefix $(OPT_CC_DEF),$(ADEFINES))
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC1)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC2)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC3)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC4)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC5)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC6)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC7)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC8)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(CC) $(CAFLAGS)$(call adjpath,$@) $(CAINCS) $(CADEFS) $(call adjpath,$<)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$*$(EXT_INT))
	-$(RM) $*$(EXT_INT)

else

# Assemble: create object files from assembler source files.
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC1)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC2)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC3)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC4)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC5)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC6)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC7)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)
$(DIR_OBJ)/%$(EXT_OBJ) : $(DIR_SRC8)%$(EXT_ASM) $(COMMONDEP) $(DIR_OBJ)
	$(AS) $(call make_asflags,$@) $(AINCS) $(ADEFS) $(call adjpath,$<)

endif

endif
