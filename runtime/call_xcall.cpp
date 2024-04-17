#include "call_xcall.h"
#include "host_cdts_converter.h"
#include "py_tuple.h"
#include "utils.h"
#include <stdexcept>

PyObject* call_xcall(void* pxcall, void* pcontext, PyObject* param_metaffi_types, PyObject* retval_metaffi_types, PyObject* args)
{
	pyscope();

	Py_ssize_t retval_count = py_tuple::get_size(retval_metaffi_types);
	
	py_tuple param_metaffi_types_tuple(param_metaffi_types);
	Py_ssize_t params_count = param_metaffi_types_tuple.size();
	
	std::vector<metaffi_type_info> param_metaffi_types_vec(params_count);
	
	for(int i=0 ; i<params_count ; i++)
	{
		PyObject* item = param_metaffi_types_tuple[i];
		if(!PyLong_Check(item))
		{
			PyErr_SetString(PyExc_ValueError, "param_metaffi_types must be a tuple of integers");
			return Py_None;
		}
		param_metaffi_types_vec[i] = (metaffi_type_info)PyLong_AsLong(item);
	}
	
	if (params_count > 0 || retval_count > 0)
	{
		cdts* pcdts = convert_host_params_to_cdts(args, param_metaffi_types_vec.data(), params_count, retval_count);
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

