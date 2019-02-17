#
# Copyright (c) 2019, Ari Suutari <ari@stonepile.fi>.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. The name of the author may not be used to endorse or promote
#     products derived from this software without specific prior written
#     permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

set(CMAKE_SYSTEM_PROCESSOR ARM)

# which compilers to use for C and C++

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# search settings

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)

# find additional toolchain executables

find_program(ARM_SIZE_EXECUTABLE arm-none-eabi-size)
find_program(ARM_GDB_EXECUTABLE arm-none-eabi-gdb)
find_program(ARM_OBJCOPY_EXECUTABLE arm-none-eabi-objcopy)
find_program(ARM_OBJDUMP_EXECUTABLE arm-none-eabi-objdump)

# CPU setting

set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-${CORTEX} -mthumb")

if(CORTEX STREQUAL "m4")

  string(APPEND CMAKE_C_FLAGS_INIT " -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

endif()

if(NEWLIB_NANO)

  string(APPEND CMAKE_C_FLAGS_INIT " --specs=nano.specs")

endif()

# Common GNU settings for all ports

include(ToolchainGNU-Common)

# Linker settings to create image.

set(CMAKE_EXE_LINKER_FLAGS_INIT "-L${CMAKE_CURRENT_SOURCE_DIR}")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -L${CMAKE_CURRENT_LIST_DIR}/boot")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -T${LD_SCRIPTS} -mcpu=cortex-${CORTEX}")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -nostartfiles")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,-Map,${PROJECT_NAME}.map,--cref,")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT "--wrap=malloc,--wrap=free,--wrap=realloc,")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT "--wrap=__sfp_lock_acquire,--wrap=__sfp_lock_release,")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT "--wrap=fopen")

