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
#  $Id: Makefile,v 1.1 2006/04/29 15:33:21 dkuschel Exp $


PORT = arm
CPU  = lpc2000
BUILD = DEBUG
NANO = 1

# this is for olimex board
LD_SCRIPT  = lpc2129-rom.ld
BOARD = BOARD_OLIMEX

# this is for embedded artist's board
#LD_SCRIPT  = lpc2138-rom.ld
#BOARD = BOARD_EA

# Set root path and include base make file
RELROOT = ../../../
include $(RELROOT)make/common.mak

CDEFINES += $(BOARD)

# --------------------------------------------------------------------------

# Set target file name
TARGET = demo

# Set source files
SRC_TXT = demo.c
SRC_OBJ =
SRC_LIB =

# Set the directory that contains the configuration header files
#DIR_CONFIG = ports\6502\test

# ---------------------------------------------------------------------------

POSTLINK1 = arm-elf-size $(TARGETOUT)
POSTLINK2 = arm-elf-objcopy -O ihex $(TARGETOUT) $(TARGET).hex
# Build an executable
include $(MAKE_OUT)

