cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# why does cmake strips runtime paths ?
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wunused -Wextra -g")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wunused -Wextra -g -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

message("-- CMAKE_CXX_FLAGS_DEBUG : ${CMAKE_CXX_FLAGS_DEBUG}")
message("-- CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")

project(igclib VERSION 0.1.1)
configure_file("${PROJECT_SOURCE_DIR}/include/igclib/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/igclib/config.hpp")

# find boost libraries
find_package(Boost REQUIRED)

# disable library tests
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(BUILD_CPR_TESTS OFF CACHE INTERNAL "")
set(USE_SYSTEM_CURL ON)

# add libraries
add_subdirectory(third-party/geographiclib)
add_subdirectory(third-party/json)
add_subdirectory(third-party/cpr)
add_subdirectory(third-party/dlib)

# add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

enable_testing()