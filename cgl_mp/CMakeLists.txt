set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
include_directories("../include")
add_executable(cgl_mp cgl_mp.cpp cgl_mp.hpp partition.hpp)
target_link_libraries(cgl_mp cgl)

# testing
add_executable(cgl_mp_test cgl_mp_test.cpp cgl_mp.hpp partition.hpp)
target_link_libraries(cgl_mp_test cgl)

#install(TARGETS cgl_mp RUNTIME DESTINATION bin)
