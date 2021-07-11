#pragma once
#include <runtime/cdt_structs.h>
#include <Python.h>

extern "C"
{
	cdt* convert_host_params_to_cdts(PyObject* params_names, PyObject* params_types);
	PyObject* convert_host_return_values_from_cdts(cdt* cdt_return_values, openffi_size return_values_count);
}