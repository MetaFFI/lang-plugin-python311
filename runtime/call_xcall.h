#pragma once
#include <runtime/cdt.h>
#include <runtime/xcall.h>

// Forward declaration - PyObject is defined in Python.h, but we don't want to include it in the header
struct _object;
typedef struct _object PyObject;

extern "C"
{
    PyObject* call_xcall(void* pxcall_ptr, void* context, PyObject* param_metaffi_types, PyObject* retval_metaffi_types, PyObject* args);
}