cmake_minimum_required(VERSION 3.5)

project(client VERSION 2.0.0 LANGUAGES C CXX)
set(PROJECT_HOME ${client_SOURCE_DIR})
set(PRONAME client)

if(USE32)
  message(STATUS "Using 32 Bits")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  set(LINKSUFFIX 32)
else()
  message(STATUS "Using 64 Bits")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
  set(LINKSUFFIX 64)
endif(USE32)

include_directories(${PROJECT_HOME}/../inc)
link_directories(${PROJECT_HOME}/../lib${LINKSUFFIX})

add_compile_options(-std=c++11)
add_executable(${PRONAME} main)
target_link_libraries(${PRONAME} ecauth ecutility glog pthread)
