
# add SDK
if(NOT DEFINED sdk_src)
	message(FATAL_ERROR "sdk_src is not defined. Ensure sdk/CMakeLists.txt is added before lang-plugin-python3.")
endif()

# runtime
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/runtime")

# compiler IDL plugin
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/idl")

# compiler plugin
#add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/compiler")

add_custom_target(python3
	DEPENDS xllr.python3 metaffi.idl.python3 #metaffi.compiler.python3
)
set_target_properties(python3 PROPERTIES EXCLUDE_FROM_ALL TRUE)

# set python3 to parent scope
set(python3 ${python3} PARENT_SCOPE)
