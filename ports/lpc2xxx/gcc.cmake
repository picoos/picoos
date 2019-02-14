set(CMAKE_SYSTEM_PROCESSOR ARM)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_OBJDUMP arm-none-eabi-objdump)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CFLAGS_COMMON "-DGCC")

if(THUMB STREQUAL "yes")

  set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mthumb-interwork")
  set(THUMB_FLAGS "-mthumb -mthumb-interwork")

else()

  set(THUMB_FLAGS "-mno-thumb-interwork")

endif()

set(CFLAGS_COMMON "${CFLAGS_COMMON} ${THUMB_FLAGS}")
set(CFLAGS_COMMON "${CFLAGS_COMMON} -fno-common -mcpu=arm7tdmi -ffunction-sections")
set(CFLAGS_COMMON "${CFLAGS_COMMON} -Wcast-align -Wall -Wextra -Wshadow -Wpointer-arith")
set(CFLAGS_COMMON "${CFLAGS_COMMON} -Waggregate-return -Wmissing-declarations")
set(CFLAGS_COMMON "${CFLAGS_COMMON} -Wno-unused-parameter -Wno-unused-label -Wno-unused-but-set-variable")

set(CFLAGS_COMMON_RELEASE "-O2 -fomit-frame-pointer -D_REL")
set(CFLAGS_COMMON_DEBUG "-g -D_DBG")

set(CMAKE_C_FLAGS "-Wbad-function-cast -Wno-strict-prototypes -Wmissing-prototypes ${CFLAGS_COMMON} ${EXTRA_CFLAGS}" CACHE INTERNAL "c compiler flags")
set(CMAKE_C_FLAGS_DEBUG "${CFLAGS_COMMON_DEBUG}" CACHE INTERNAL "c compiler flags: Debug")
set(CMAKE_C_FLAGS_RELEASE "${CFLAGS_COMMON_RELEASE}" CACHE INTERNAL "c compiler flags: Release")

set(CMAKE_CXX_FLAGS "-fno-exceptions -fno-rtti ${CFLAGS_COMMON} ${EXTRA_CFLAGS}" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE INTERNAL "cxx compiler flags: Debug")
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "cxx compiler flags: Release")

set(CMAKE_ASM_FLAGS "-mcpu=arm7tdmi -Wa,-gstabs -Wa,-I${CURDIR}/ports/arm -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "" CACHE INTERNAL "asm compiler flags: Debug")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm compiler flags: Release")


set(CMAKE_EXE_LINKER_FLAGS "${THUMB_FLAGS} -L${CMAKE_CURRENT_SOURCE_DIR} -L${CMAKE_CURRENT_LIST_DIR}/boot  -T${LD_SCRIPTS} -mcpu=arm7tdmi")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles -Wl,-Map,${PROJECT_NAME}.map,--cref,--gc-sections")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)

# find additional toolchain executables
find_program(ARM_SIZE_EXECUTABLE arm-none-eabi-size)
find_program(ARM_GDB_EXECUTABLE arm-none-eabi-gdb)
find_program(ARM_OBJCOPY_EXECUTABLE arm-none-eabi-objcopy)
find_program(ARM_OBJDUMP_EXECUTABLE arm-none-eabi-objdump)
