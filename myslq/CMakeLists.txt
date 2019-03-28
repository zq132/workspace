cmake_minimum_required(VERSION 3.5)

if (POLICY CMP0042)
  cmake_policy (SET CMP0042 NEW)
  message(STATUS "cmp0042")
endif (POLICY CMP0042)

if (POLICY CMP0063)
  cmake_policy (SET CMP0063 NEW)
  message(STATUS "cmp0063")
endif (POLICY CMP0063)

project(ec-register VERSION 2.0.0 LANGUAGES C CXX)
set(PROJECT_HOME ${ec-register_SOURCE_DIR})
set(PRONAME myslq)

if(USE32)
  message(STATUS "Using 32 Bits")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  set(OUTPUT_HOME ${PROJECT_HOME}/../lib32)
  set(LIBRARY_OUTPUT_PATH ${OUTPUT_HOME})
  set(LINKSUFFIX 32)
else()
  message(STATUS "Using 64 Bits")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
  set(OUTPUT_HOME ${PROJECT_HOME}/../lib64)
  set(LIBRARY_OUTPUT_PATH ${OUTPUT_HOME})
  set(LINKSUFFIX 64)
endif(USE32)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-D__Debug)
  set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}d)
  message(STATUS "Debug model.")
else()
  message(STATUS "Release model.")
endif()

set(ECLIBS ${PROJECT_HOME}/../lib${LINKSUFFIX})

include_directories(inc ${PROJECT_HOME}/../inc)
link_directories(lib ${ECLIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_HOME}/../bin${LINKSUFFIX})

add_compile_options(-std=c++11)
add_executable(${PRONAME} systemD server)
target_link_libraries(${PRONAME} ecportcheck ecutility glog pthread)

