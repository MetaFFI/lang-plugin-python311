find_package(Boost REQUIRED COMPONENTS filesystem) # Boost library
find_package(Python3 REQUIRED COMPONENTS Development) # Python library

set(target_name "xllr.python3")

# build xllr.python3
file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp ${CMAKE_CURRENT_LIST_DIR}/*.h ${METAFFI_SDK}/utils/*.cpp ${METAFFI_SDK}/runtime/*.cpp ${METAFFI_SDK}/runtime/*.c ${METAFFI_SDK}/utils/*.hpp)
add_library(${target_name}
			SHARED
			${SRC_FILES}
)
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS} ${METAFFI_SDK}/runtime/*.h ${METAFFI_SDK}/utils/*.h)

set_property(TARGET ${target_name} PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.python3.[ext]"
SET_TARGET_PROPERTIES(${target_name} PROPERTIES PREFIX "")

target_link_libraries(${target_name} Boost::filesystem Python3::Python)
target_link_directories(${target_name} PUBLIC ${Python3_LIBRARY_DIRS})

if(WIN32)
	target_link_directories(${target_name} PUBLIC ${BOOST_LIBRARYDIR})
else()
	target_link_libraries(${target_name} ${Boost_LIBRARIES})
endif()

# Copy to metaffi directory
add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		"$ENV{METAFFI_HOME}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		COMMENT "Copy ${CMAKE_CURRENT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX} -> $ENV{METAFFI_HOME}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Unitest Python
#file(GLOB TEST_SRC_FILES ${METAFFI_SDK}/utils/*.cpp ${CMAKE_CURRENT_LIST_DIR}/xllr_python3_test.cpp)
#add_executable(xllr_python3_test ${TEST_SRC_FILES})
#target_link_libraries(xllr_python3_test Boost::filesystem dl)
#add_test(NAME test_python_runtime_success COMMAND xllr_python3_test 1)
#add_test(NAME test_python_module_success COMMAND xllr_python3_test 2)
#add_test(NAME test_python_module_free_module_via_free_runtime_success COMMAND xllr_python3_test 3)
#add_test(NAME test_python_module_lazy_runtime_success COMMAND xllr_python3_test 4)
#add_test(NAME test_python_module_module_not_exist_fail COMMAND xllr_python3_test 5)
#add_test(NAME test_python_call_success COMMAND xllr_python3_test 6)
#add_test(NAME test_python_call_fail COMMAND xllr_python3_test 7)


