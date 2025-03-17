#pragma once
#include <runtime/cdt.h>
#include <runtime/xcall.h>

// In order not to require python debug library
#include "python3_api_wrapper.h"

extern "C"
{
    PyObject* call_xcall(void* pxcall_ptr, void* context, PyObject* param_metaffi_types, PyObject* retval_metaffi_types, PyObject* args);
}