# IDL Plugin for Python 3
# Builds a dynamic library that loads Python dynamically and implements the IDL plugin interface

# Find required dependencies
find_or_install_package(doctest)
find_or_install_package(Boost COMPONENTS filesystem)

# Collect C++ source files
collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} metaffi_idl_python3)
# metaffi_idl_python3_src is now set by the macro

# Collect SDK cpython3 runtime_manager sources
collect_c_cpp_files("${metaffi_sdk_root}/runtime_manager/cpython3" sdk_cpython3_runtime_manager)

# Combined SDK sources for cpython3
set(sdk_cpython3_src
	${sdk_src}
	${sdk_cpython3_runtime_manager_src}
)

# SDK include directories (add metaffi_sdk_root for <runtime_manager/cpython3/...> style includes)
set(sdk_cpython3_include
	${sdk_include_dir}
	${metaffi_sdk_root}
)

# Platform-specific link libraries
set(platform_libs "")
if(WIN32)
	# Version.lib is needed for GetFileVersionInfoA, VerQueryValueA, etc.
	# used in python_api_wrapper.cpp for detecting Python version from DLL
	set(platform_libs "Version.lib")
endif()

# Build the IDL plugin as a dynamic library
# NO Python link required - Python is loaded dynamically at runtime
c_cpp_shared_lib(metaffi.idl.python3
	"${metaffi_idl_python3_src};${sdk_cpython3_src}"
	"${sdk_cpython3_include};${Boost_INCLUDE_DIRS}"
	"Boost::filesystem;${platform_libs}"
	"./python3")

set(metaffi_idl_python3 metaffi.idl.python3 PARENT_SCOPE)
