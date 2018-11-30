project (client)
cmake_minimum_required(VERSION 3.5)

set(LIBRARY_OUTPUT_PATH ${client_SOURCE_DIR}/lib)

#include_directories(${client_SOURCE_DIR}/inc)
link_directories(${client_SOURCE_DIR}/lib)

add_compile_options(-std=c++11)
add_library(ecauth SHARED Auth)
target_link_libraries(ecauth ecutility glog pthread)
#target_link_libraries(portcheck ${DIR_SRC})
add_executable(client main)
target_link_libraries(client ecauth)

