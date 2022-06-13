#pragma once
#include <runtime/cdt_structs.h>

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
	cdt* convert_host_params_to_cdts(PyObject* params_names, PyObject* params_types);
	PyObject* convert_host_return_values_from_cdts(cdt* cdt_return_values, metaffi_size return_values_count);
}