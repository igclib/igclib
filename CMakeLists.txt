cmake_minimum_required(VERSION 3.10)

project(igclib VERSION 0.1.0)
configure_file("${PROJECT_SOURCE_DIR}/include/igclib/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/igclib/config.hpp")

set(cmake_cxx_standard 11)
set(cmake_cxx_standard_required True)

# why does cmake strips runtime paths ?
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror  -Wunused -Wextra -g -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
message("-- CMAKE_CXX_FLAGS_DEBUG : ${CMAKE_CXX_FLAGS_DEBUG}")
message("-- CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")

# enable testing
enable_testing()

# find boost libraries
find_package(Boost REQUIRED)

# disable json library tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# add libraries
add_subdirectory(include/geographiclib EXCLUDE_FROM_ALL)
add_subdirectory(include/json EXCLUDE_FROM_ALL)
add_subdirectory(include/cpr EXCLUDE_FROM_ALL)

# add subdirectories
add_subdirectory(src)
add_subdirectory(tests)