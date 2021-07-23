#pragma once
#include <Python.h>
#include <runtime/openffi_primitives.h>

extern "C" PyObject* new_py_openffi_handle(openffi_handle h);