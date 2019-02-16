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

# Linker settings

string(APPEND CMAKE_ASM_FLAGS_INIT " -x assembler-with-cpp")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--gc-sections")


