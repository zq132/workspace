project (eccheck)
cmake_minimum_required(VERSION 3.5)

set(LIBRARY_OUTPUT_PATH ${eccheck_SOURCE_DIR}/lib)

include_directories(${eccheck_SOURCE_DIR}/inc)
link_directories(${eccheck_SOURCE_DIR}/lib)

add_compile_options(-std=c++11)
#add_library(ecutility SHARED utility)
#target_link_libraries(ecutility libglog.a)
#target_link_libraries(portcheck ${DIR_SRC})
add_executable(myslq systemD server)
target_link_libraries(myslq ecportcheck ecutility glog pthread)

