include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CPR_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../third-party/json/single-include)


set(BINARY ${CMAKE_PROJECT_NAME})
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

add_library(lib_${BINARY} ${SOURCES})
target_link_libraries(lib_${BINARY} ${Boost_LIBRARIES})
target_link_libraries(lib_${BINARY} ${CPR_LIBRARIES})
target_link_libraries(lib_${BINARY} GeographicLib)
target_link_libraries(lib_${BINARY} nlohmann_json::nlohmann_json)
target_link_libraries(lib_${BINARY} ${CERES_LIBRARIES})

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} lib_${BINARY})

install(TARGETS ${BINARY} DESTINATION bin)