include_guard(GLOBAL)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/ports/${PORT}/gcc.cmake CACHE PATH "toolchain file")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_SYSTEM_NAME      picoOS)
set(CMAKE_SYSTEM_VERSION   1)

include(${CMAKE_CURRENT_LIST_DIR}/ports/${PORT}/port.cmake)
