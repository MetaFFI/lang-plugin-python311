#include "host_cdts_converter.h"
#include "runtime_globals.h"
#include <iostream>

// SDK includes
#include <cdts_serializer/cpython3/cdts_python3_serializer.h>
#include <runtime_manager/cpython3/gil_guard.h>
#include <runtime/xllr_capi_loader.h>

using namespace metaffi::utils;

//--------------------------------------------------------------------
[[maybe_unused]] cdts* convert_host_params_to_cdts(PyObject* params, metaffi_type_info* param_metaffi_types, metaffi_size params_count, metaffi_size return_values_size)
{
	gil_guard guard;
	cdts* cdts_buf = nullptr;
	try
	{
		std::cerr << "[DEBUG] convert_host_params_to_cdts: params_count=" << params_count << ", return_values_size=" << return_values_size << std::endl;

		Py_ssize_t params_size = Py_IsNone(params) ? 0 : pPyTuple_Size(params);
		std::cerr << "[DEBUG] convert_host_params_to_cdts: params_size=" << params_size << std::endl;
		if(params_size == 0)
		{
			if(return_values_size == 0)
			{
				return nullptr;
			}
			return xllr_alloc_cdts_buffer(0, return_values_size);
		}

		// Allocate CDTS buffer
		cdts_buf = xllr_alloc_cdts_buffer(pPyTuple_Size(params), return_values_size);
		std::cerr << "[DEBUG] convert_host_params_to_cdts: allocated cdts_buf=" << (void*)cdts_buf << std::endl;
		cdts& cdts_params = cdts_buf[0];

		// Use SDK serializer to convert Python objects to CDTS
		cdts_python3_serializer ser(get_runtime_manager(), cdts_params);
		for(metaffi_size i = 0; i < params_count; i++)
		{
			std::cerr << "[DEBUG] convert_host_params_to_cdts: serializing param[" << i << "], type=" << param_metaffi_types[i].type << std::endl;
			PyObject* item = pPyTuple_GetItem(params, i);
			ser.add(item, param_metaffi_types[i].type);
			std::cerr << "[DEBUG] convert_host_params_to_cdts: param[" << i << "] serialized successfully" << std::endl;
		}

		std::cerr << "[DEBUG] convert_host_params_to_cdts: all params serialized successfully" << std::endl;
		return cdts_buf;
	}
	catch(std::exception& e)
	{
		std::cerr << "[DEBUG] convert_host_params_to_cdts: EXCEPTION: " << e.what() << std::endl;
		
		// Free allocated buffer if serialization failed
		if(cdts_buf != nullptr)
		{
			xllr_free_cdts_buffer(cdts_buf);
			cdts_buf = nullptr;
		}
		
		std::stringstream ss;
		ss << "Failed convert_host_params_to_cdts: " << e.what();

		pPyErr_SetString(pPyExc_RuntimeError, ss.str().c_str());
		return nullptr;
	}
}
//--------------------------------------------------------------------
[[maybe_unused]] PyObject* convert_host_return_values_from_cdts(cdts* pcdts, metaffi_size index)
{
	gil_guard guard;

	try
	{
		std::cerr << "[DEBUG] convert_host_return_values_from_cdts: index=" << index << std::endl;
		
		cdts& retvals = pcdts[index];
		std::cerr << "[DEBUG] convert_host_return_values_from_cdts: retvals.length=" << retvals.length << ", retvals.arr=" << (void*)retvals.arr << std::endl;

		if(retvals.arr && retvals.length > 0)
		{
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: retvals[0].type=" << retvals.arr[0].type << std::endl;
			std::cerr.flush();
		}
		else
		{
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: WARNING - retvals is empty or null!" << std::endl;
			std::cerr.flush();
		}

		// Use SDK serializer to convert CDTS to Python tuple
		// Check if retvals is valid before accessing length
		std::cerr << "[DEBUG] convert_host_return_values_from_cdts: about to create serializer" << std::endl;
		std::cerr.flush();
		std::cerr << "[DEBUG] convert_host_return_values_from_cdts: retvals.length=" << retvals.length << std::endl;
		std::cerr.flush();
		
		try
		{
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: getting runtime manager" << std::endl;
			std::cerr.flush();
			auto& rt_mgr = get_runtime_manager();
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: runtime manager obtained, creating serializer" << std::endl;
			std::cerr.flush();
			cdts_python3_serializer ser(rt_mgr, retvals);
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: serializer created, calling extract_as_tuple" << std::endl;
			std::cerr.flush();
			PyObject* result = ser.extract_as_tuple(); //<---- HERE
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: extract_as_tuple returned " << (void*)result << std::endl;
			std::cerr.flush();
			return result;
		}
		catch(const std::exception& e)
		{
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: EXCEPTION during serializer/extract: " << e.what() << std::endl;
			std::cerr.flush();
			throw;
		}
		catch(...)
		{
			std::cerr << "[DEBUG] convert_host_return_values_from_cdts: UNKNOWN EXCEPTION during serializer/extract" << std::endl;
			std::cerr.flush();
			throw;
		}
	}
	catch(std::exception& e)
	{
		std::cerr << "[DEBUG] convert_host_return_values_from_cdts: EXCEPTION: " << e.what() << std::endl;
		std::stringstream ss;
		ss << "Failed convert_host_return_values_from_cdts: " << e.what();
		pPyErr_SetString(pPyExc_RuntimeError, ss.str().c_str());
		return nullptr;
	}
}
//--------------------------------------------------------------------
