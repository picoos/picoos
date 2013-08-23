#
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
#  $Id: makefile,v 1.4 2006/03/14 18:21:16 dkuschel Exp $


# This port is avr
PORT = avr

# output format for the binary (srec, ihex, binary)
OUTPUT_FORMAT = ihex

# extension of the output
OUTPUT_EXT = hex

# Set compiler for this project:
#  GNU C    = GCC
COMPILER = GCC

# Set root path and include base make file
RELROOT = ../../../
include $(RELROOT)make/common.mak

# --------------------------------------------------------------------------

# Set target file name
TARGET = test

# Set source files
SRC_TXT = rectangle.c
SRC_OBJ =
SRC_LIB =

# Set the directory that contains the configuration header files.
# If this variable is not set, the default configuration files will be
# taken from the port/default directory.
#DIR_CONFIG = $(CURRENTDIR)

# Set the output directory for the generated binaries
DIR_OUTPUT = $(CURRENTDIR)/bin


# ---- Configure the linker port hook --------------------------------------
# generate the cof-file for the debugging in AvrStudio and 
# the binary for the programming device.

OBJCOPY = avr-objcopy

OBJDUMP = avr-objdump

COEFFICIENTS = --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 


POSTLINK1 =	$(OBJCOPY) $(COEFFICIENTS) -O coff-avr $(TARGETOUT) $(DIR_OUT)/$(TARGET).cof
POSTLINK2 = $(OBJCOPY) -O $(OUTPUT_FORMAT) -R .eeprom $(TARGETOUT) $(DIR_OUT)/$(TARGET).$(OUTPUT_EXT)
POSTLINK3 = $(OBJDUMP) -h -S $(TARGETOUT) > $(DIR_OUT)/$(TARGET).lss


# ---------------------------------------------------------------------------

# Build an executable
include $(MAKE_OUT)

