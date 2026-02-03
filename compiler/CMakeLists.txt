# Compiler Plugin for Python 3
# Builds a dynamic library that loads Python dynamically and implements the compiler plugin interface

# Find required dependencies
find_or_install_package(Boost COMPONENTS filesystem)

# Collect C++ source files from this directory
collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} metaffi_compiler_python3)
# metaffi_compiler_python3_src is now set by the macro

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

# Build the compiler plugin as a dynamic library
# NO Python link required - Python is loaded dynamically at runtime
c_cpp_shared_lib(metaffi.compiler.python3
	"${metaffi_compiler_python3_src};${sdk_cpython3_src}"
	"${sdk_cpython3_include};${Boost_INCLUDE_DIRS}"
	"Boost::filesystem;${platform_libs}"
	"./python3")

# Copy Python module dependencies to $METAFFI_HOME/python3/compiler/
# These are needed at runtime for the host compiler to work
set(METAFFI_HOME_PATH $ENV{METAFFI_HOME})
if(NOT METAFFI_HOME_PATH)
	message(FATAL "METAFFI_HOME environment variable not set. Python SDK modules will not be copied.")
endif()

set(COMPILER_DEPS_DIR "${METAFFI_HOME_PATH}/python3/compiler")

add_custom_command(TARGET metaffi.compiler.python3 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E echo "Copying Python SDK compiler modules to ${COMPILER_DEPS_DIR}"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${COMPILER_DEPS_DIR}/sdk/compiler/python3"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${COMPILER_DEPS_DIR}/sdk/idl_entities/python3"

	# Copy sdk/compiler/python3/ contents
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${metaffi_sdk_root}/compiler/python3"
		"${COMPILER_DEPS_DIR}/sdk/compiler/python3"

	# Copy sdk/idl_entities/python3/ contents
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${metaffi_sdk_root}/idl_entities/python3"
		"${COMPILER_DEPS_DIR}/sdk/idl_entities/python3"

	COMMAND ${CMAKE_COMMAND} -E echo "Python SDK compiler modules copied successfully"
)

set(metaffi_compiler_python3 metaffi.compiler.python3 PARENT_SCOPE)

# Add E2E tests subdirectory
add_subdirectory(test)
