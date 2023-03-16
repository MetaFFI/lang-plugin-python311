
#------------------------------- "py" IDL Plugin ------------------------------------------

# TODO: This uses MetaFFI and "Go" metaffi plugin.
# TODO: Add metaffi ability to check if metaffi plugin is installed

set(target_name "metaffi.idl.py")
add_go_target(${target_name})
add_dependencies(${target_name} metaffi.compiler.python3)

include(${CMAKE_SCRIPTS_DIR}/MetaFFI.cmake)

# Build py_extractor MetaFFI files
metaffi_compile_guest(${target_name} "py_extractor.json" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
metaffi_compile_host(${target_name} "py_extractor.json" "go" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

go_get(${target_name} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
go_build(${target_name} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_go_test("py IDL plugin test")

# install
install(FILES   ${PROJECT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
				${CMAKE_CURRENT_LIST_DIR}/py_extractor.py
				${CMAKE_CURRENT_LIST_DIR}/py_extractor_MetaFFIGuest.py
		DESTINATION .)