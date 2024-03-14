#include "call_xcall.h"
#include <stdexcept>
#include "host_cdts_converter.h"
#include "utils.h"

PyObject* call_xcall(void* pxcall, void* pcontext, metaffi_type_info* param_metaffi_types, uint64_t params_count, metaffi_type_info* retval_metaffi_types, uint64_t retval_count, PyObject* args)
{
	pyscope();

	if (params_count > 0 || retval_count > 0)
	{
		cdts* pcdts = convert_host_params_to_cdts(args, param_metaffi_types, params_count, retval_count);
		if(pcdts == nullptr)
		{
			return Py_None;
		}

		char* out_err = nullptr;
		uint64_t out_err_len = 0;

		((void(*)(void*,cdts*,char**,uint64_t*))pxcall)(pcontext, pcdts, &out_err, &out_err_len);

		if(out_err_len > 0)
		{
			PyErr_SetString(PyExc_ValueError, std::string(out_err, out_err_len).c_str());
			return Py_None;
		}

		if(retval_count == 0) {
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

