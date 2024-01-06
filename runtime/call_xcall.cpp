#include "call_xcall.h"
#include <stdexcept>
#include "host_cdts_converter.h"
#include "utils.h"

PyObject* call_xcall(void* pxcall, void* pcontext, PyObject* tuple_param_metaffi_types, PyObject* tuple_retval_metaffi_types, PyObject* args)
{
	pyscope();

	if (PyTuple_Size(tuple_param_metaffi_types) > 0 || PyTuple_Size(tuple_retval_metaffi_types) > 0)
	{
		cdts* pcdts = convert_host_params_to_cdts(args, tuple_param_metaffi_types, PyTuple_Size(tuple_retval_metaffi_types));

		char* out_err = nullptr;
		uint64_t out_err_len = 0;

		((void(*)(void*,cdts*,char**,uint64_t*))pxcall)(pcontext, pcdts, &out_err, &out_err_len);

		if (out_err_len > 0)
		{
			PyErr_SetString(PyExc_ValueError, std::string(out_err, out_err_len).c_str());
			return Py_None;
		}

		if (PyTuple_Size(tuple_retval_metaffi_types) == 0) {
			return Py_None;
		}

		return convert_host_return_values_from_cdts(pcdts, 1);
	}
	else
	{
		char* out_err = nullptr;
		uint64_t out_err_len = 0;
		((void(*)(void*,char**,uint64_t*))pxcall)(pcontext, &out_err, &out_err_len);
		if (out_err_len > 0)
		{
			PyErr_SetString(PyExc_ValueError, std::string(out_err, out_err_len).c_str());
			return Py_None;
		}

		return Py_None;
	}
}

