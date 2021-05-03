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

include_guard(GLOBAL)

enable_language(ASM)

# Pico layer files.

set(FILES_CORE src/pico/picoos.c)
if(GENERIC_FINDBIT)

  list(APPEND FILES_CORE src/pico/fbit_gen.c)

endif()

# Nano layer files.

if (NANO)

  file(GLOB FILES_NANO src/nano/*.c)
  set(DEFS_NANO POSNANO)

endif()

# Port files

file(GLOB FILES_PORT ports/${PORT}/*.c
                     ports/${PORT}/*.s
                     ports/${PORT}/boot/*.c
                     ports/${PORT}/boot/*.s)

if (CPU)

  file(GLOB FILES_CPU ports/${PORT}/${CPU}/*.c
                      ports/${PORT}/${CPU}/*.s
                      ports/${PORT}/${CPU}/boot/*.c
                      ports/${PORT}/${CPU}/boot/*.s)

endif()

if(CMSIS_MODULES)

  add_subdirectory(${CMSIS_MODULES} cmsis)

endif()

if(HAL_MODULES)

  foreach(M ${HAL_MODULES})
     add_peer_directory(${M})
  endforeach()

endif()

# Create picoos library

add_library(picoos STATIC ${FILES_CORE} ${FILES_NANO} ${FILES_PORT} ${FILES_CPU})
target_include_directories(picoos BEFORE PUBLIC inc ${DIR_CONFIG} ports/${PORT})

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  target_compile_options(picoos PRIVATE -Wno-parentheses-equality)
endif()

target_compile_definitions(picoos PUBLIC _POSPACK ${DEFS_NANO})

if(CMSIS_MODULES)

  target_link_libraries(picoos cmsis)

endif()

if(HAL_MODULES)

  foreach(M ${HAL_MODULES})
     get_filename_component(BASE ${M} NAME)
     target_link_libraries(picoos ${BASE})
  endforeach()

endif()

# Documentation

find_package(Doxygen)
if (DOXYGEN_FOUND)

  configure_file(make/doxygen.cfg.in doxygen.cfg)
  add_custom_target(picoos-docs
        COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/doc/html
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating dox." )

else()

  message(STATUS "Doxygen missing.")
endif()
