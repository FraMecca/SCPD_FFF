set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
include_directories("../include")
add_library(cgl cgl.hpp cgl_eval.hpp)
# testing
include_directories("../include")
add_executable(cgl_test cgl_test.cpp)
target_link_libraries(cgl_test cgl)
