#include "host_cdts_converter.h"
#include "runtime_globals.h"
#include <utils/logger.hpp>

// SDK includes
#include <cdts_serializer/cpython3/cdts_python3_serializer.h>
#include <runtime_manager/cpython3/gil_guard.h>
#include <runtime/xllr_capi_loader.h>

using namespace metaffi::utils;
static spdlog::logger* LOG = metaffi::get_logger("python3.runtime");

//--------------------------------------------------------------------
[[maybe_unused]] cdts* convert_host_params_to_cdts(PyObject* params, metaffi_type_info* param_metaffi_types, metaffi_size params_count, metaffi_size return_values_size)
{
	gil_guard guard;
	cdts* cdts_buf = nullptr;
	try
	{
		METAFFI_DEBUG(LOG, "convert_host_params_to_cdts: params_count={}, return_values_size={}", params_count, return_values_size);

		Py_ssize_t params_size = Py_IsNone(params) ? 0 : pPyTuple_Size(params);
		METAFFI_DEBUG(LOG, "convert_host_params_to_cdts: params_size={}", params_size);
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
		METAFFI_DEBUG(LOG, "convert_host_params_to_cdts: allocated cdts_buf={}", static_cast<void*>(cdts_buf));
		cdts& cdts_params = cdts_buf[0];

		// Use SDK serializer to convert Python objects to CDTS
		cdts_python3_serializer ser(get_runtime_manager(), cdts_params);
		for(metaffi_size i = 0; i < params_count; i++)
		{
			METAFFI_DEBUG(LOG, "convert_host_params_to_cdts: serializing param[{}], type={}", i, param_metaffi_types[i].type);
			PyObject* item = pPyTuple_GetItem(params, i);
			ser.add(item, param_metaffi_types[i].type);
			METAFFI_DEBUG(LOG, "convert_host_params_to_cdts: param[{}] serialized successfully", i);
		}

		METAFFI_DEBUG(LOG, "convert_host_params_to_cdts: all params serialized successfully");
		return cdts_buf;
	}
	catch(std::exception& e)
	{
		(void)e;
		METAFFI_DEBUG(LOG, "convert_host_params_to_cdts: EXCEPTION: {}", e.what());
		
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
		METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: index={}", index);
		
		cdts& retvals = pcdts[index];
		METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: retvals.length={}, retvals.arr={}",
			retvals.length, static_cast<void*>(retvals.arr));

		if(retvals.arr && retvals.length > 0)
		{
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: retvals[0].type={}", retvals.arr[0].type);
		}
		else
		{
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: WARNING - retvals is empty or null!");
		}

		// Use SDK serializer to convert CDTS to Python tuple
		// Check if retvals is valid before accessing length
		METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: about to create serializer");
		METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: retvals.length={}", retvals.length);
		
		try
		{
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: getting runtime manager");
			auto& rt_mgr = get_runtime_manager();
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: runtime manager obtained, creating serializer");
			cdts_python3_serializer ser(rt_mgr, retvals);
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: serializer created, calling extract_as_tuple");
			PyObject* result = ser.extract_as_tuple(); //<---- HERE
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: extract_as_tuple returned {}",
				static_cast<void*>(result));
			return result;
		}
		catch(const std::exception& e)
		{
			(void)e;
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: EXCEPTION during serializer/extract: {}", e.what());
			throw;
		}
		catch(...)
		{
			METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: UNKNOWN EXCEPTION during serializer/extract");
			throw;
		}
	}
	catch(std::exception& e)
	{
		(void)e;
		METAFFI_DEBUG(LOG, "convert_host_return_values_from_cdts: EXCEPTION: {}", e.what());
		std::stringstream ss;
		ss << "Failed convert_host_return_values_from_cdts: " << e.what();
		pPyErr_SetString(pPyExc_RuntimeError, ss.str().c_str());
		return nullptr;
	}
}
//--------------------------------------------------------------------
