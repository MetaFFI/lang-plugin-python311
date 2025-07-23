# IDL Plugin for Python 3.11
# Builds a dynamic library that embeds Python and implements the IDL plugin interface

# Find required dependencies
find_or_install_package(doctest)
find_or_install_package(Boost COMPONENTS filesystem)

# Collect C++ source files
collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} metaffi_idl_python311)
set(metaffi_idl_python311_src ${NEW_FILES})

# Build the IDL plugin as a dynamic library
c_cpp_shared_lib(metaffi.idl.python311
    "${metaffi_idl_python311_src};${sdk_src};../runtime/python3_api_wrapper.cpp"
    "${sdk_include_dir};${Boost_INCLUDE_DIRS};${CPython_INCLUDE_DIR};../runtime"
    "Boost::filesystem;"
    "./python311")

set(metaffi_idl_python311 metaffi.idl.python311 PARENT_SCOPE)

# Add test executable
c_cpp_exe(python311_idl_plugin_test
    "idl_plugin_test.cpp;python_idl_plugin.cpp;../plugin-sdk/runtime/xllr_capi_loader.c;../runtime/python3_api_wrapper.cpp"
    "${sdk_include_dir};${Boost_INCLUDE_DIRS};${doctest_INCLUDE_DIRS};${CPython_INCLUDE_DIR};../runtime"
    "doctest::doctest;Boost::filesystem;"
    ".")

add_dependencies(python311_idl_plugin_test metaffi.idl.python311)
set(python311_idl_plugin_test python311_idl_plugin_test PARENT_SCOPE)

# Add test
add_test(NAME idl_plugin_test COMMAND $ENV{METAFFI_HOME}/idl_plugin_test) 