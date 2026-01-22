


# * ---- API unitest ----
add_test(NAME python3_api_unitest
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_EXECUTABLE_ARG} -m unittest test_python3_api.py
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/unittest
)

# * ---- Cross Language tests ----

add_test(NAME python3_api_cross_pl_tests
		COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_EXECUTABLE_ARG} run_api_tests.py
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
)


# * ---- Publish python metaffi package to pypi ----
add_custom_target(python3.publish
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_EXECUTABLE_ARG} publish_python_package.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS xllr.python3
)
