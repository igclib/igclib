file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test_*.cpp)

# add test data
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

foreach(file ${TEST_SOURCES})
	string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" test_file ${file})
	add_executable(${test_file} ${file})
	target_link_libraries(${test_file} lib_${PROJECT_NAME})
	add_test(${test_file} ${test_file})

	set_tests_properties(${test_file}
		PROPERTIES
		PASS_REGULAR_EXPRESSION ".*passed.*")
	set_tests_properties(${test_file}
		PROPERTIES
		FAIL_REGULAR_EXPRESSION ".*failed.*")
	set_tests_properties(${test_file}
		PROPERTIES
		TIMEOUT 60)
endforeach()
