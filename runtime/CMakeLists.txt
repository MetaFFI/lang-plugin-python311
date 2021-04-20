find_package(Boost REQUIRED COMPONENTS filesystem) # Boost library
find_package(Python3 REQUIRED COMPONENTS Development) # Python library

set(target_name "xllr.python3")

# build xllr.python3
file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp ${CMAKE_CURRENT_LIST_DIR}/*.h ${OPENFFI_SDK}/utils/*.cpp)
add_library(${target_name}
			SHARED
			${SRC_FILES}
)
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})

set_property(TARGET ${target_name} PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.python3.[ext]"
SET_TARGET_PROPERTIES(${target_name} PROPERTIES PREFIX "")

# get the name of the library and use this to link
# it is to ignore full path of package managers like vcpkg

get_filename_component(py3Lib "${Python3_LIBRARIES}" NAME)

find_library(isFoundNoFullPath ${py3Lib})
if (isFoundNoFullPath)
	target_link_libraries(${target_name} ${py3Lib} Boost::filesystem)
else()
	target_link_libraries(${target_name} ${Python3_LIBRARIES} Boost::filesystem)
endif()

# Copy to openffi directory
add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		"$ENV{OPENFFI_HOME}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		COMMENT "Copy ${CMAKE_CURRENT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX} -> $ENV{OPENFFI_HOME}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Unitest Python
#file(GLOB TEST_SRC_FILES ${OPENFFI_SDK}/utils/*.cpp ${CMAKE_CURRENT_LIST_DIR}/xllr_python3_test.cpp)
#add_executable(xllr_python3_test ${TEST_SRC_FILES})
#target_link_libraries(xllr_python3_test Boost::filesystem dl)
#add_test(NAME test_python_runtime_success COMMAND xllr_python3_test 1)
#add_test(NAME test_python_module_success COMMAND xllr_python3_test 2)
#add_test(NAME test_python_module_free_module_via_free_runtime_success COMMAND xllr_python3_test 3)
#add_test(NAME test_python_module_lazy_runtime_success COMMAND xllr_python3_test 4)
#add_test(NAME test_python_module_module_not_exist_fail COMMAND xllr_python3_test 5)
#add_test(NAME test_python_call_success COMMAND xllr_python3_test 6)
#add_test(NAME test_python_call_fail COMMAND xllr_python3_test 7)


