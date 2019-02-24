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

cmake_minimum_required(VERSION 3.13)

set(PORT unix)
set(CPU 32)
set(NANO 1)

include(../../cmake/ToolchainInit.cmake)
message("XXX FIX THIS ${CMAKE_C_COMPILER_ID}")
set(DIR_CONFIG ${PICOOS_DIR}/ports/${PORT}/default)

macro(add_example src)

  add_executable(${src} ${src}.c)
  target_link_libraries(${src} picoos)

endmacro()

project(examples)

add_peer_directory(${PICOOS_DIR})

if(NOT NANO)
add_example(test)
add_example(ex_task1)
add_example(ex_task2)
add_example(ex_task3)

else()

add_example(ex_bhalf)
add_example(ex_flag1)
add_example(ex_flag2)
add_example(ex_lists)
add_example(ex_mesg1)
add_example(ex_mesg2)
add_example(ex_mutx1)
add_example(ex_mutx2)
add_example(ex_sema1)
add_example(ex_sema2)
add_example(ex_sema3)
add_example(ex_sema4)
add_example(ex_sint1)
add_example(ex_task4)
add_example(ex_timr1)
add_example(ex_timr2)
add_example(ex_sema5)
add_example(ex_task5)
add_example(ex_flag3)

endif()
