# E2E Tests for Python3 Host Compiler
#
# This test suite validates the Python3 host compiler by:
# 1. Running the metaffi compiler to generate host code from xllr.test IDL
# 2. Using pytest to verify the generated code works correctly with xllr.test

add_py_test(python3.host_compiler_test
    MODULE test_host_compiler_e2e
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

set(python3.host_compiler_test ${python3.host_compiler_test} PARENT_SCOPE)