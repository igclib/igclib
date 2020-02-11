cmake_minimum_required(VERSION 3.10)

project(igclib)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

set(cmake_cxx_standard 11)
set(cmake_cxx_standard_required True)

add_executable(igclib ${SOURCES})