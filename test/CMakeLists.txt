# Python3 host tests (plugin-level)

find_or_install_package(Boost COMPONENTS filesystem)
find_or_install_package(doctest)

set(python3_host_test_src
	${CMAKE_CURRENT_LIST_DIR}/test_main.cpp
	${CMAKE_CURRENT_LIST_DIR}/python3_test_env.cpp
	${CMAKE_CURRENT_LIST_DIR}/python3_wrappers.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_core.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_classes.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_arrays.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_args.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_module_state.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_async.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_single_file.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_third_party.cpp
)

set(python3_host_test_includes
	${sdk_include_dir}
	${metaffi_sdk_root}/api/cpp/include
	${CMAKE_CURRENT_LIST_DIR}
	${doctest_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

set(python3_host_test_libs
	doctest::doctest
	Boost::filesystem
	metaffi.api.cpp
)

c_cpp_exe(python3_host_test
	"${python3_host_test_src}"
	"${python3_host_test_includes}"
	"${python3_host_test_libs}"
	"$ENV{METAFFI_HOME}"
)

add_test(NAME python3_host_test
		COMMAND $ENV{METAFFI_HOME}/python3_host_test${CMAKE_EXECUTABLE_SUFFIX})
if(WIN32)
	set(python3_host_test_path "$ENV{METAFFI_HOME}/sdk/api/cpp;$ENV{PATH}")
	string(REPLACE ";" "\\;" python3_host_test_path "${python3_host_test_path}")
	set_tests_properties(python3_host_test PROPERTIES
		ENVIRONMENT "PATH=${python3_host_test_path}"
	)
elseif(APPLE)
	set_tests_properties(python3_host_test PROPERTIES
		ENVIRONMENT "DYLD_LIBRARY_PATH=$ENV{METAFFI_HOME}/sdk/api/cpp:$ENV{DYLD_LIBRARY_PATH}"
	)
else()
	set_tests_properties(python3_host_test PROPERTIES
		ENVIRONMENT "LD_LIBRARY_PATH=$ENV{METAFFI_HOME}/sdk/api/cpp:$ENV{LD_LIBRARY_PATH}"
	)
endif()
set(python3_host_test python3_host_test PARENT_SCOPE)
