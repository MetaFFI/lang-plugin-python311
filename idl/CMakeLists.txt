# IDL Plugin for Python 3.11
# Builds a dynamic library that embeds Python and implements the IDL plugin interface

# Find required dependencies
find_or_install_package(doctest)
find_or_install_package(Boost COMPONENTS filesystem)

# Collect C++ source files
collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} metaffi_idl_python3)
set(metaffi_idl_python3_src ${NEW_FILES})

# Build the IDL plugin as a dynamic library
c_cpp_shared_lib(metaffi.idl.python3
    "${metaffi_idl_python3_src};${sdk_src};../runtime/python3_api_wrapper.cpp"
    "${sdk_include_dir};${Boost_INCLUDE_DIRS};${CPython_INCLUDE_DIR};../runtime"
    "Boost::filesystem;"
    "./python3")

set(metaffi_idl_python3 metaffi.idl.python3 PARENT_SCOPE)

# Add test executable
c_cpp_exe(python3_idl_plugin_test
    "idl_plugin_test.cpp;python_idl_plugin.cpp;../../sdk/runtime/xllr_capi_loader.c;../runtime/python3_api_wrapper.cpp"
    "${sdk_include_dir};${Boost_INCLUDE_DIRS};${doctest_INCLUDE_DIRS};${CPython_INCLUDE_DIR};../runtime"
    "doctest::doctest;Boost::filesystem;"
    "./python3")

add_dependencies(python3_idl_plugin_test metaffi.idl.python3)
set(python3_idl_plugin_test python3_idl_plugin_test PARENT_SCOPE)

# Add test
add_test(NAME idl_plugin_test COMMAND $ENV{METAFFI_HOME}/python3/idl_plugin_test) 
