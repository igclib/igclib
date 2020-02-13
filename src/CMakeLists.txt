set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

add_executable(${BINARY} ${SOURCES})
#add_library(lib_${BINARY} STATIC ${SOURCES})
