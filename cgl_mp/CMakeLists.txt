add_executable(cgl_mp cgl_mp.cpp partition.hpp) 
#include_directories(${CMAKE_SOURCE_DIR}/cgl)
target_link_libraries(cgl_mp cgl)

#install(TARGETS cgl_mp RUNTIME DESTINATION bin)
