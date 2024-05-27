
#------------------------------- Python3 Language Support - Runtime Plugin ------------------------------------------


set(target_name "xllr.python311")

# build xllr.python3
get_dir_sources(${CMAKE_CURRENT_LIST_DIR} ${target_name}_sources)
get_sdk_utils_sources(utils_srcs)
get_sdk_runtime_sources(runtime_srcs)

add_library(${target_name} SHARED ${${target_name}_sources} ${utils_srcs} ${runtime_srcs})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${METAFFI_SDK} ${Python3_INCLUDE_DIRS})
set_target_properties(${target_name} PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON LINK_FLAGS "-Wl,-Bsymbolic")
target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} Boost::filesystem ${Python3_LIBRARY_RELEASE})

set_target_properties( ${target_name}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
		LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
		RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
		)

# install
install(CODE "message(STATUS \"Installing xllr.python3 to ${CMAKE_BINARY_DIR}\")")
install_target(${target_name} .)

# in the install, copy the generated file to METAFFI_HOME environment variable directory
# make sure to fix the backslashes in METAFFI_HOME environment variables to forward slashes
copy_file_from_project_binary_to_metaffi_home(${target_name} ${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX})

# unitests
add_executable(python_api_test "python_runtime_test.cpp" ${utils_srcs} ${runtime_srcs})
target_link_libraries(python_api_test PRIVATE doctest::doctest ${target_name} Boost::filesystem ${Python3_LIBRARY_RELEASE})
target_include_directories(python_api_test PRIVATE ${METAFFI_SDK} ${Python3_INCLUDE_DIRS})
if(NOT WIN32)
	add_custom_command(TARGET python_api_test POST_BUILD COMMAND patchelf --set-rpath "\"$ORIGIN:$ORIGIN/lib:$ENV{METAFFI_HOME}:${Boost_LIBRARY_DIRS}\"" $<TARGET_FILE:python_api_test>)
endif()

add_test(NAME "Python3 Runtime API" COMMAND python_api_test)

