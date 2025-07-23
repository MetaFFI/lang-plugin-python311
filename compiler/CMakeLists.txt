# load conan packages
find_or_install_package(Boost COMPONENTS filesystem)
find_or_install_package(doctest)

# build Go metaffi.compiler.python311 dynamic library
go_build(metaffi.compiler.python311
		SOURCE_DIR
			"${CMAKE_CURRENT_LIST_DIR}"
		OUTPUT_DIR
			"./python311"
		OUTPUT_NAME
			metaffi.compiler.python311
		DEPENDENT
			${CMAKE_CURRENT_LIST_DIR}
)
set(metaffi.compiler.python311 metaffi.compiler.python311 PARENT_SCOPE)

# go test for metaffi.compiler.python311
add_test(NAME metaffi_compiler_python311_test
		COMMAND go test
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)