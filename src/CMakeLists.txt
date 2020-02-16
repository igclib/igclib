include_directories(${CMAKE_SOURCE_DIR}/include)

set(BINARY ${CMAKE_PROJECT_NAME})
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} GeographicLib)
target_link_libraries(${BINARY} nlohmann_json::nlohmann_json)

add_library(lib_${BINARY} STATIC ${SOURCES})
target_link_libraries(lib_${BINARY} GeographicLib)
target_link_libraries(lib_${BINARY} nlohmann_json::nlohmann_json)

install(TARGETS ${BINARY} DESTINATION bin)