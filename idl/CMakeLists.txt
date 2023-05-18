
#------------------------------- "py" IDL Plugin ------------------------------------------

set(target_name "metaffi.idl.py")
add_go_target(${target_name})
add_dependencies(${target_name} metaffi.compiler.python3)

include(${CMAKE_SCRIPTS_DIR}/MetaFFI.cmake)

metaffi_confirm_plugin("go runtime")
metaffi_confirm_plugin("go compiler")

# Build py_extractor MetaFFI files
metaffi_compile_guest(${target_name} "py_extractor.json" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
metaffi_compile_host(${target_name} "py_extractor.json" "go" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

go_get(${target_name} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
go_build(${target_name} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_go_test("py IDL plugin test")

copy_file_post_build(${target_name} ${CMAKE_CURRENT_LIST_DIR}/py_extractor.py ${PROJECT_BINARY_DIR}/py_extractor.py)
copy_file_post_build(${target_name} ${CMAKE_CURRENT_LIST_DIR}/py_extractor_MetaFFIGuest.py ${PROJECT_BINARY_DIR}/py_extractor_MetaFFIGuest.py)

# install
install(FILES   ${PROJECT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
				${CMAKE_CURRENT_LIST_DIR}/py_extractor.py
				${CMAKE_CURRENT_LIST_DIR}/py_extractor_MetaFFIGuest.py
				${CMAKE_CURRENT_LIST_DIR}/none_python_impl_definitions.py
		DESTINATION .)