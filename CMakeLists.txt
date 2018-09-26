cmake_minimum_required(VERSION 3.5)
project(CGL)

set (CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    # default is release
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type (Debug,Release)" FORCE)
endif()

message("Building configuration: " ${CMAKE_BUILD_TYPE})

# release is optimized
set (CMAKE_CXX_FLAGS_RELEASE "-O2")
# debug includes gdb info
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wpedantic -g -ggdb")
# profiling
if(PROF)
    message("Profiling enabled")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pg")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
endif()

set (CGL_VERSION_MAJOR 0)
set (CGL_VERSION_MINOR 1)

add_subdirectory(cgl)
add_subdirectory(cgl_seq)
#add_subdirectory(cgl_mp)

set_target_properties(cgl PROPERTIES LINKER_LANGUAGE CXX)

# clear cache
set (CMAKE_BUILD_TYPE "" CACHE STRING "Default build type" FORCE)
option (PROF "Profiling with gprof" OFF)
