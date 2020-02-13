cmake_minimum_required(VERSION 3.10)

project(igclib)
enable_testing()

set(cmake_cxx_standard 11)
set(cmake_cxx_standard_required True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror  -Wunused -Wextra -g")

find_package(Boost REQUIRED)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    include_directories(include)
    
    add_subdirectory(src)
    #add_subdirectory(tests)
else()
  message( FATAL_ERROR "Boost libraries are required. Install them and try again." )
endif()


