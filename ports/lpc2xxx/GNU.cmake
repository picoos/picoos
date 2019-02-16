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

set(CMAKE_C_FLAGS_INIT "-mcpu=arm7tdmi")
set(CMAKE_ASM_FLAGS_INIT "-mcpu=arm7tdmi")

# Handle thumb/arm instruction set selection

if(THUMB)

  string(APPEND CMAKE_ASM_FLAGS_INIT " -mthumb-interwork")
  string(APPEND CMAKE_C_FLAGS_INIT " -mthumb -mthumb-interwork")

else()

  string(APPEND CMAKE_C_FLAGS_INIT " -mno-thumb-interwork")

endif()

# Common GNU settings for all ports

include(ToolchainGNU-Common)

# Linker settings to create image.

set(CMAKE_EXE_LINKER_FLAGS_INIT "-L${CMAKE_CURRENT_SOURCE_DIR}")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -L${CMAKE_CURRENT_LIST_DIR}/boot")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -T${LD_SCRIPTS} -mcpu=arm7tdmi")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -nostartfiles")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,-Map,${PROJECT_NAME}.map,")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT "--cref")

