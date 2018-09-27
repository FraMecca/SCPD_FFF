set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
include_directories("../include")
add_executable(cgl_seq cgl_main.cpp) 
target_link_libraries(cgl_seq cgl)
