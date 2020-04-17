cmake_minimum_required(VERSION 3.10)

enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# why does cmake strips runtime paths ?
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wunused -Wextra -g -DDLIB_USE_CUDA=0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wunused -Wextra -g -pedantic")

message("-- CMAKE_CXX_FLAGS_DEBUG : ${CMAKE_CXX_FLAGS_DEBUG}")
message("-- CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")

project(igclib VERSION 0.1.1)
configure_file("${PROJECT_SOURCE_DIR}/include/igclib/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/igclib/config.hpp")

# find boost libraries
find_package(Boost COMPONENTS system filesystem REQUIRED)

# disable third-party tests
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(BUILD_CPR_TESTS OFF CACHE INTERNAL "")
set(USE_SYSTEM_CURL ON)

# add third-party libraries
add_subdirectory(third-party/json)
add_subdirectory(third-party/cpr)
add_subdirectory(third-party/geographiclib)
add_subdirectory(third-party/dlib)

# add source and tests
add_subdirectory(src)
add_subdirectory(tests)
