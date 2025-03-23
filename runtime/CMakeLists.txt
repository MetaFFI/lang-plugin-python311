# load conan packages
find_or_install_package(Boost COMPONENTS filesystem)
find_or_install_package(doctest)

collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} xllr.python311)
remove_files_inside_path("${xllr.python311_src}" "deps")
set(xllr.python311_src ${NEW_FILES})

# get cpython from "deps" as vcpkg doesn't have python3.11 and python3.12
c_cpp_shared_lib(xllr.python311
		"${xllr.python311_src};${sdk_src}"
		"${sdk_include_dir};${Boost_INCLUDE_DIRS};${CPython_INCLUDE_DIR}"
		"Boost::filesystem;"
		"./python311")

set(xllr.python311 xllr.python311 PARENT_SCOPE)

# run python_runtime_test.cpp doctest unit test
c_cpp_exe(python_runtime_test
		"${xllr.python311_src};${sdk_src};python_runtime_test.cpp"
		"${sdk_include_dir};${Boost_INCLUDE_DIRS};${doctest_INCLUDE_DIRS};${CPython_INCLUDE_DIR}"
		"doctest::doctest;Boost::filesystem;"
		".")

# add post build that deletes libpython3* from $ENV{METAFFI_HOME}/python311
add_custom_command(TARGET python_runtime_test POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove
		$ENV{METAFFI_HOME}/python311/libpython3*
		COMMENT "Deleting libpython3* from $ENV{METAFFI_HOME}/python311")


add_dependencies(python_runtime_test xllr.python311)
set(python_runtime_test python_runtime_test PARENT_SCOPE)

add_test(NAME python_runtime_test COMMAND $ENV{METAFFI_HOME}/python_runtime_test)