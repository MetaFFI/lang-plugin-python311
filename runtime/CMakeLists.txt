
#------------------------------- Python3 Language Support - Runtime Plugin ------------------------------------------

add_boost(filesystem)
add_python3()

set(target_name "xllr.python311")

# build xllr.python3
get_dir_sources(${CMAKE_CURRENT_LIST_DIR} ${target_name}_sources)
get_sdk_utils_sources(utils_srcs)
get_sdk_runtime_sources(runtime_srcs)

add_library(${target_name} SHARED ${${target_name}_sources} ${utils_srcs} ${runtime_srcs})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${METAFFI_SDK})
set_target_properties(${target_name} PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON LINK_FLAGS "-Wl,-Bsymbolic")
target_link_libraries(${target_name} PRIVATE Boost::filesystem Python3::Python)
link_to_dl_for_non_windows(${target_name})

set_target_properties( ${target_name}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
		LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
		RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
		)

# install
install_target(${target_name} .)


# unitests
add_executable(python_api_test "python_api_test.cpp" ${utils_srcs} ${runtime_srcs})
target_link_libraries(python_api_test PRIVATE Catch2::Catch2 ${target_name} Boost::filesystem Python3::Python)
target_include_directories(python_api_test PRIVATE ${METAFFI_SDK})
if(NOT WIN32)
	add_custom_command(TARGET python_api_test POST_BUILD COMMAND patchelf --set-rpath "$ORIGIN:$ORIGIN/lib:$ENV{METAFFI_HOME}:${Boost_LIBRARY_DIRS}" $<TARGET_FILE:python_api_test>)
endif()

add_test(NAME "Python3 Runtime API" COMMAND python_api_test)


add_executable(cdts_python3_test "cdts_python3_test.cpp" ${utils_srcs} ${runtime_srcs})
target_link_libraries(cdts_python3_test PRIVATE Catch2::Catch2 ${target_name} Boost::filesystem Python3::Python)
target_include_directories(cdts_python3_test PRIVATE ${METAFFI_SDK})
if(NOT WIN32)
	add_custom_command(TARGET cdts_python3_test POST_BUILD COMMAND patchelf --set-rpath "$ORIGIN:$ORIGIN/lib:$ENV{METAFFI_HOME}:${Boost_LIBRARY_DIRS}" $<TARGET_FILE:python_api_test>)
endif()

add_test(NAME "Python3 CDTS" COMMAND cdts_python3_test)
