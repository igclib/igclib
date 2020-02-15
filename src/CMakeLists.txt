file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

set(BINARY ${CMAKE_PROJECT_NAME})
add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} GeographicLib)
set_target_properties(${BINARY} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install(TARGETS ${BINARY} DESTINATION bin)

add_library(lib_${BINARY} STATIC ${SOURCES})
