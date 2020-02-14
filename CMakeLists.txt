cmake_minimum_required(VERSION 3.10)

project(igclib)

# Version information
set (PROJECT_VERSION_MAJOR 0)
set (PROJECT_VERSION_MINOR 1)
set (PROJECT_VERSION_PATCH 0)
set (PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
configure_file("${PROJECT_SOURCE_DIR}/include/igclib/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/igclib/config.hpp")


set(cmake_cxx_standard 11)
set(cmake_cxx_standard_required True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror  -Wunused -Wextra -g")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(Boost REQUIRED)
find_package (GeographicLib REQUIRED)

include(CTest)
include_directories("include")
link_libraries(${GeographicLib_LIBRARIES})

add_subdirectory(src)
add_subdirectory(tests)