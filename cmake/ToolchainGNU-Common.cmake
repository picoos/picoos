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

string(APPEND CMAKE_C_FLAGS_INIT " -DGCC")

# use function-sections (and gc-sections in linker) so
# unused code is discarded.
# Consider -fdata-sections here (however it seems to make code larger)

string(APPEND CMAKE_C_FLAGS_INIT " -fno-common -ffunction-sections")

# Setup compiler warnings

string(APPEND CMAKE_C_FLAGS_INIT " -Wcast-align -Wall -Wextra -Wshadow -Wpointer-arith")
string(APPEND CMAKE_C_FLAGS_INIT " -Waggregate-return -Wmissing-declarations")
string(APPEND CMAKE_C_FLAGS_INIT " -Wno-unused-parameter -Wno-unused-label")
string(APPEND CMAKE_C_FLAGS_INIT " -Wno-unused-but-set-variable")

# Debug/Release -specfic flags

set(CMAKE_C_FLAGS_RELEASE_INIT "-fomit-frame-pointer -D_REL")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-fomit-frame-pointer -D_REL")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-fomit-frame-pointer -D_REL")
set(CMAKE_C_FLAGS_DEBUG_INIT "-D_DBG")

# Copy current C settings to C++

set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_C_FLAGS_RELEASE_INIT}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "${CMAKE_C_FLAGS_MINSIZEREL_INIT}")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_C_FLAGS_DEBUG_INIT}")

# C specific settings

string(APPEND CMAKE_C_FLAGS_INIT " -Wbad-function-cast -Wno-strict-prototypes")
string(APPEND CMAKE_C_FLAGS_INIT " -Wmissing-prototypes")

# C++ specific settings

string(APPEND CMAKE_CXX_FLAGS_INIT " -fno-exceptions -fno-rtti")

# Assembler settings

string(APPEND CMAKE_ASM_FLAGS_INIT " -x assembler-with-cpp")

# Linker settings

set(CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--gc-sections")


