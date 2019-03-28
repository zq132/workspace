project(createSerialNo)
cmake_minimum_required(VERSION 3.5)

add_compile_options(-std=c++11)
link_directories(lib)
#add_library(creatserial SHARED crypto)
add_executable(creser mainSerial)
target_link_libraries(creser eccreatekey)
