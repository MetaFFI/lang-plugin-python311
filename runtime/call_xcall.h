#pragma once
#include <runtime/cdt.h>

// In order not to require python debug library
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

extern "C"
{
    PyObject* call_xcall(void* pxcall, void* pcontext, metaffi_type_info* param_metaffi_types, uint64_t params_count, metaffi_type_info* retval_metaffi_types, uint64_t retval_count, PyObject* args);
}