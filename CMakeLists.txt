

# add SDK
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/plugin-sdk")

# runtime
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/runtime")

# api
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/api")

add_custom_target(python311 ALL
	DEPENDS xllr.python311
)