#include "host_cdts_converter.h"
#include "runtime_globals.h"

// SDK includes
#include <cdts_serializer/cpython3/cdts_python3_serializer.h>
#include <runtime_manager/cpython3/gil_guard.h>
#include <runtime/xllr_capi_loader.h>

using namespace metaffi::utils;

//--------------------------------------------------------------------
[[maybe_unused]] cdts* convert_host_params_to_cdts(PyObject* params, metaffi_type_info* param_metaffi_types, metaffi_size params_count, metaffi_size return_values_size)
{
	try
	{
		gil_guard guard;

		Py_ssize_t params_size = Py_IsNone(params) ? 0 : pPyTuple_Size(params);
		if(params_size == 0)
		{
			if(return_values_size == 0)
			{
				return nullptr;
			}
			return xllr_alloc_cdts_buffer(0, return_values_size);
		}

		// Allocate CDTS buffer
		cdts* cdts_buf = xllr_alloc_cdts_buffer(pPyTuple_Size(params), return_values_size);
		cdts& cdts_params = cdts_buf[0];

		// Use SDK serializer to convert Python objects to CDTS
		cdts_python3_serializer ser(get_runtime_manager(), cdts_params);
		for(metaffi_size i = 0; i < params_count; i++)
		{
			PyObject* item = pPyTuple_GetItem(params, i);
			ser.add(item, param_metaffi_types[i].type);
		}

		return cdts_buf;
	}
	catch(std::exception& e)
	{
		std::stringstream ss;
		ss << "Failed convert_host_params_to_cdts: " << e.what();

		pPyErr_SetString(NULL, ss.str().c_str());
		return nullptr;
	}
}
//--------------------------------------------------------------------
[[maybe_unused]] PyObject* convert_host_return_values_from_cdts(cdts* pcdts, metaffi_size index)
{
	try
	{
		gil_guard guard;
		cdts& retvals = pcdts[index];

		// Use SDK serializer to convert CDTS to Python tuple
		cdts_python3_serializer ser(get_runtime_manager(), retvals);
		return ser.extract_as_tuple();
	}
	catch(std::exception& e)
	{
		std::stringstream ss;
		ss << "Failed convert_host_return_values_from_cdts: " << e.what();
		pPyErr_SetString(NULL, ss.str().c_str());
		return pPy_None;
	}
}
//--------------------------------------------------------------------
