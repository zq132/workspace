project(createAuthKey)
cmake_minimum_required(VERSION 3.5)
set(PROJECT_HOME ${createAuthKey_SOURCE_DIR}) 

if(USE32)
  message(STATUS "Using 32bits")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  #set(OUTPUT_HOME ${PROJECT_HOME}/lib32)
  set(LIBRARY_OUTPUT_PATH ${PROJECT_HOME}/lib32)
else()
  message(STATUS "Using 64bits")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
  #set(OUTPUT_HOME ${PROJECT_HOME}/lib64)
  set(LIBRARY_OUTPUT_PATH ${PROJECT_HOME}/lib64)
endif(USE32)

#set(LIBRARY_OUTPUT_PATH ../)

add_compile_options(-std=c++11)
#link_directories(lib)
add_library(eccreatekey SHARED crypto)
target_link_libraries(eccreatekey crypto)
add_executable(crekey mainKey)
target_link_libraries(crekey eccreatekey)
