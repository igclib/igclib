cmake_minimum_required(VERSION 3.10)

project(igclib)
enable_testing()

set(cmake_cxx_standard 11)
set(cmake_cxx_standard_required True)

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wextra -g")
