include_guard(GLOBAL)

enable_language(ASM)
set(FILES_CORE src/pico/picoos.c)
if(GENERIC_FINDBIT EQUAL 1)

  set(FILES_FINDBIT src/pico/fbit_gen.c)

endif()

if (NANO EQUAL 1)

  file(GLOB FILES_NANO src/nano/*.c)
  set(DEFS_NANO POSNANO)

endif()

file(GLOB FILES_PORT ports/${PORT}/*.c ports/${PORT}/*.s ports/${PORT}/boot/*.c ports/${PORT}/boot/*.s)

if (${CPU})

  file(GLOB FILES_CPU ports/${PORT}/${CPU}/*.c ports/${PORT}/${CPU}/*.s ports/${PORT}/${CPU}/boot/*.c ports/${PORT}/${CPU}/boot/*.s)

endif()

add_library(picoos STATIC ${FILES_CORE} ${FILES_FINDBIT} ${FILES_NANO} ${FILES_PORT} ${FILES_CPU})
target_include_directories(picoos BEFORE PUBLIC inc ${DIR_CONFIG} ports/${PORT})
target_compile_definitions(picoos PUBLIC _POSPACK ${DEFS_NANO})
