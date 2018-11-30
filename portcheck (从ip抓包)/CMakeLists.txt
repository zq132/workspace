project (portcheck)
cmake_minimum_required(VERSION 3.5)

set(LIBRARY_OUTPUT_PATH ${portcheck_SOURCE_DIR}/lib)

include_directories(${portcheck_SOURCE_DIR}/inc)
#link_directories(${portcheck_SOURCE_DIR}/lib)

add_compile_options(-std=c++11)
add_library(ecportcheck SHARED portlisten crypto)
target_link_libraries(ecportcheck crypto pthread)
#target_link_libraries(portcheck ${DIR_SRC})
add_executable(porttest main)
target_link_libraries(porttest ecportcheck)

