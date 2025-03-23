#include "call_xcall.h"
#include "host_cdts_converter.h"
#include "py_int.h"
#include "py_tuple.h"
#include "utils.h"
#include <mutex>
#include <stdexcept>

std::once_flag load_python_api_flag;

PyObject* call_xcall(void* pxcall_ptr, void* context, PyObject* param_metaffi_types, PyObject* retval_metaffi_types, PyObject* args)
{
	if(pxcall_ptr == nullptr)
	{
		pPyErr_SetString(NULL, "xcall is null");
		return pPy_None;
	}
	
	// Thread-safe check and call to load_python3_api()
	std::call_once(load_python_api_flag, []()
	{
		if (pPy_IsInitialized == nullptr || pPy_IsInitialized() == 0)
		{
			printf("Python is not initialized!\n");
			throw std::runtime_error("Python is not initialized!");
		}
	});

	xcall pxcall(pxcall_ptr, context);

	PyGILState_STATE gstate = pPyGILState_Ensure();
	metaffi::utils::scope_guard sggstate([&]() { pPyGILState_Release(gstate);} );
	
	Py_ssize_t retval_count = py_tuple::get_size(retval_metaffi_types);
	
	py_tuple param_metaffi_types_tuple(param_metaffi_types);
	Py_ssize_t params_count = param_metaffi_types_tuple.size();

	std::vector<metaffi_type_info> param_metaffi_types_vec(params_count);
	
	for(int i=0 ; i<params_count ; i++)
	{
		PyObject* item = param_metaffi_types_tuple[i];
		if( py_object::get_object_type(item) == "metaffi_type_info") // <---- fix ME!!!! search any use of tp_name and fix it
		{
			// Get the fields of the metaffi_type_info instance
			PyObject* type = pPyObject_GetAttrString(item, "type");
			PyObject* alias = pPyObject_GetAttrString(item, "alias");
			PyObject* dimensions = pPyObject_GetAttrString(item, "fixed_dimensions");
		
			// Convert the fields to C types
			uint64_t type_c = pPyLong_AsUnsignedLongLong(type);
			char* alias_c = Py_IsNone(alias) ? nullptr : (char*)pPyUnicode_AsUTF8(alias);
			int64_t dimensions_c = pPyLong_AsLong(dimensions);
		
			if(pPyErr_Occurred())
			{
				return pPy_None;
			}
			
			// Create a metaffi_type_info struct and populate it with the field values
			metaffi_type_info info;
			info.type = type_c;
			info.alias = alias_c;
			info.is_free_alias = alias_c != nullptr ? 1 : 0;
			info.fixed_dimensions = dimensions_c;
		
			param_metaffi_types_vec[i] = std::move(info);
		
			// Decrement the reference counts of the field objects
			Py_DECREF(type);
			Py_DECREF(alias);
			Py_DECREF(dimensions);
			
		}
		else if(py_int::check(item))
		{
			uint64_t type_c = pPyLong_AsUnsignedLongLong(item);
			metaffi_type_info info;
			info.type = type_c;
			info.alias = nullptr;
			info.is_free_alias = 0;
			info.fixed_dimensions = 0;
			param_metaffi_types_vec[i] = std::move(info);
		}
		else
		{
			pPyErr_SetString(NULL, "expected metaffi_type_info");
			return pPy_None;
		}
	}
	
	if (params_count > 0 || retval_count > 0)
	{
		cdts* pcdts = convert_host_params_to_cdts(args, param_metaffi_types_vec.data(), params_count, retval_count);
		if(pcdts == nullptr)
		{
			return pPy_None;
		}

		char* out_err = nullptr;
		
		pxcall(pcdts, &out_err);

		if(out_err)
		{
			pPyErr_SetString(NULL, out_err);
			xllr_free_string(out_err);
			return pPy_None;
		}

		if(retval_count == 0) {
			return pPy_None;
		}

		return convert_host_return_values_from_cdts(pcdts, 1);
	}
	else
	{
		char* out_err = nullptr;
		pxcall(&out_err);
		if (out_err)
		{
			pPyErr_SetString(NULL, out_err);
			xllr_free_string(out_err);
			return pPy_None;
		}

		return pPy_None;
	}
}

