include_guard(GLOBAL)
set(PICOOS_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(PORT_DIR ${PICOOS_DIR}/ports/${PORT})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

include(${PORT_DIR}/port.cmake)

if(DEFINED PORT_DEFAULT_TOOLCHAIN)
  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${PORT_DIR}/${PORT_DEFAULT_TOOLCHAIN}.cmake CACHE PATH "toolchain file")
  endif()
endif()

set(CMAKE_SYSTEM_NAME      picoOS)
set(CMAKE_SYSTEM_VERSION   1)


