project(createAuthKey)
cmake_minimum_required(VERSION 3.5)

set(LIBRARY_OUTPUT_PATH ../)

add_compile_options(-std=c++11)
#link_directories(lib)
add_library(eccreatekey SHARED crypto)
target_link_libraries(eccreatekey crypto)
add_executable(crekey mainKey)
target_link_libraries(crekey eccreatekey)
