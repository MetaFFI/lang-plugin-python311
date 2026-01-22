# load conan packages
find_or_install_package(Boost COMPONENTS filesystem)
find_or_install_package(doctest)

# build Go metaffi.compiler.python3 dynamic library
go_build(metaffi.compiler.python3
		SOURCE_DIR
			"${CMAKE_CURRENT_LIST_DIR}"
		OUTPUT_DIR
			"./python3"
		OUTPUT_NAME
			metaffi.compiler.python3
		DEPENDENT
			${CMAKE_CURRENT_LIST_DIR}
)
set(metaffi.compiler.python3 metaffi.compiler.python3 PARENT_SCOPE)

# go test for metaffi.compiler.python3
add_test(NAME metaffi_compiler_python3_test
		COMMAND go test
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
