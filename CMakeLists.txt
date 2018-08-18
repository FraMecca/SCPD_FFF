cmake_minimum_required(VERSION 3.5)
project(CGL)

set (CMAKE_CXX_STANDARD 11) 
set (CGL_VERSION_MAJOR 0)
set (CGL_VERSION_MINOR 1)

add_subdirectory(cgl)
add_subdirectory(cgl_seq)
add_subdirectory(cgl_mp)

set_target_properties(cgl PROPERTIES LINKER_LANGUAGE CXX)

