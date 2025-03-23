

# add SDK
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/plugin-sdk")

# runtime
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/runtime")

# api
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/api")

add_custom_target(python311
	DEPENDS xllr.python311 python_runtime_test
)
set_target_properties(python311 PROPERTIES EXCLUDE_FROM_ALL TRUE)

# set python311 to parent scope
set(python311 ${python311} PARENT_SCOPE)