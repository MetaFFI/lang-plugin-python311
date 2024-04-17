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
    PyObject* call_xcall(void* pxcall, void* pcontext, PyObject* param_metaffi_types, PyObject* retval_metaffi_types, PyObject* args);
}