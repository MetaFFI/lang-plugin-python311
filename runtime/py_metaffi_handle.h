#pragma once
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
#include <runtime/metaffi_primitives.h>

PyObject* new_py_metaffi_handle(metaffi_handle h, metaffi_uint64 runtime_id);