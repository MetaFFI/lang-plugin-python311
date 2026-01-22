# load conan packages
find_or_install_package(Boost COMPONENTS filesystem)
find_or_install_package(doctest)

collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} xllr.python3)
remove_files_inside_path("${xllr.python3_src}" "deps")
set(xllr.python3_src ${NEW_FILES})

# get cpython from "deps" as vcpkg doesn't have python3.11 and python3.12
c_cpp_shared_lib(xllr.python3
		"${xllr.python3_src};${sdk_src}"
		"${sdk_include_dir};${Boost_INCLUDE_DIRS};${CPython_INCLUDE_DIR}"
		"Boost::filesystem;"
		"./python3")

set(xllr.python3 xllr.python3 PARENT_SCOPE)

# run python_runtime_test.cpp doctest unit test
c_cpp_exe(python_runtime_test
		"${xllr.python3_src};${sdk_src};python_runtime_test.cpp"
		"${sdk_include_dir};${Boost_INCLUDE_DIRS};${doctest_INCLUDE_DIRS};${CPython_INCLUDE_DIR}"
		"doctest::doctest;Boost::filesystem;"
		"./python3")

# add post build that deletes libpython3* from $ENV{METAFFI_HOME}/python3
add_custom_command(TARGET python_runtime_test POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove
		$ENV{METAFFI_HOME}/python3/libpython3*
		COMMENT "Deleting libpython3* from $ENV{METAFFI_HOME}/python3")


add_dependencies(python_runtime_test xllr.python3)
set(python_runtime_test python_runtime_test PARENT_SCOPE)

add_test(NAME python_runtime_test COMMAND $ENV{METAFFI_HOME}/python3/python_runtime_test)
