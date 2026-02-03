#include "call_xcall.h"
#include "host_cdts_converter.h"
#include "runtime_globals.h"
#include <runtime_manager/cpython3/gil_guard.h>
#include <runtime_manager/cpython3/python_api_wrapper.h>
#include <runtime/xllr_capi_loader.h>
#include <utils/logger.hpp>
#include <stdexcept>
#include <runtime_manager/cpython3/py_utils.h>

static auto LOG = metaffi::get_logger("python3.runtime");

PyObject* call_xcall(void* pxcall_ptr, void* context, PyObject* param_metaffi_types, PyObject* retval_metaffi_types, PyObject* args)
{
	METAFFI_DEBUG(LOG, "call_xcall: pxcall_ptr={}, context={}", pxcall_ptr, context);
	
	if(pxcall_ptr == nullptr)
	{
		METAFFI_DEBUG(LOG, "call_xcall: pxcall_ptr is NULL!");
		pPyErr_SetString(pPyExc_RuntimeError, "xcall is null");
		Py_INCREF(pPy_None);
		return pPy_None;
	}
	
	// Acquire GIL using SDK's gil_guard
	gil_guard guard;
	
	// Ensure runtime manager is initialized
	get_runtime_manager();

	xcall pxcall(pxcall_ptr, context);
	
	METAFFI_DEBUG(LOG, "call_xcall: xcall created, pxcall_and_context[0]={}, pxcall_and_context[1]={}",
		pxcall.pxcall_and_context[0], pxcall.pxcall_and_context[1]);

	Py_ssize_t retval_count = pPyTuple_Size(retval_metaffi_types);
	
	Py_ssize_t params_count = pPyTuple_Size(param_metaffi_types);

	std::vector<metaffi_type_info> param_metaffi_types_vec(params_count);
	
	for(int i=0 ; i<params_count ; i++)
	{
		PyObject* item = pPyTuple_GetItem(param_metaffi_types, i);
		
		// Check if object is a metaffi_type_info instance (has type, alias, fixed_dimensions attributes)
		if(pPyObject_HasAttrString(item, "type") && 
		   pPyObject_HasAttrString(item, "fixed_dimensions"))
		{
			// Get the fields of the metaffi_type_info instance
			PyObject* type = pPyObject_GetAttrString(item, "type");
			if(pPyErr_Occurred())
			{
				std::string err_msg = check_python_error();
				METAFFI_ERROR(LOG, "{}", err_msg);

				Py_XDECREF(type);
				Py_INCREF(pPy_None);

				return pPy_None;
			}

			PyObject* alias = pPyObject_GetAttrString(item, "alias"); // alias its UTF-8 encoded to 'bytes'
			if(pPyErr_Occurred())
			{
				std::string err_msg = check_python_error();
				METAFFI_ERROR(LOG, "{}", err_msg);

				Py_XDECREF(type);
				Py_XDECREF(alias);
				Py_INCREF(pPy_None);

				return pPy_None;
			}

			PyObject* dimensions = pPyObject_GetAttrString(item, "fixed_dimensions");
		
			if(pPyErr_Occurred())
			{
				std::string err_msg = check_python_error();
				METAFFI_ERROR(LOG, "{}", err_msg);

				Py_XDECREF(type);
				Py_XDECREF(alias);
				Py_XDECREF(dimensions);
				Py_INCREF(pPy_None);

				return pPy_None;
			}
		
			// Convert the fields to C types
			uint64_t type_c = pPyLong_AsUnsignedLongLong(type);
			const char* alias_utf8 = nullptr;
			if(!Py_IsNone(alias))
			{
				alias_utf8 = pPyBytes_AsString(alias);

				if(pPyErr_Occurred())
				{
					Py_DECREF(type);
					Py_DECREF(alias);
					Py_DECREF(dimensions);
					Py_INCREF(pPy_None);
					return pPy_None;
				}

			}
			// Note: alias_utf8 points to internal Python string data, valid only while GIL is held
			// Since we use it immediately within the GIL guard, this is safe
			char* alias_c = alias_utf8 ? const_cast<char*>(alias_utf8) : nullptr;
			int64_t dimensions_c = pPyLong_AsLong(dimensions);
		
			if(pPyErr_Occurred())
			{
				Py_DECREF(type);
				Py_DECREF(alias);
				Py_DECREF(dimensions);
				Py_INCREF(pPy_None);
				return pPy_None;
			}
			
			// Create a metaffi_type_info struct and populate it with the field values
			metaffi_type_info info;
			info.type = type_c;
			info.alias = alias_c;
			// is_free_alias = 0 because we're using internal Python string data, not allocated memory
			info.is_free_alias = 0;
			info.fixed_dimensions = dimensions_c;
		
			param_metaffi_types_vec[i] = std::move(info);
		
			// Decrement the reference counts of the field objects
			Py_DECREF(type);
			Py_DECREF(alias);
			Py_DECREF(dimensions);
			
		}
		else if(pPyLong_Check(item))
		{
			uint64_t type_c = pPyLong_AsUnsignedLongLong(item);
			if(pPyErr_Occurred())
			{
				Py_INCREF(pPy_None);
				return pPy_None;
			}
			metaffi_type_info info;
			info.type = type_c;
			info.alias = nullptr;
			info.is_free_alias = 0;
			info.fixed_dimensions = 0;
			param_metaffi_types_vec[i] = std::move(info);
		}
		else
		{
			pPyErr_SetString(pPyExc_RuntimeError, "expected metaffi_type_info or integer");
			Py_INCREF(pPy_None);
			return pPy_None;
		}
	}
	
	if (params_count > 0 || retval_count > 0)
	{
		cdts* pcdts = convert_host_params_to_cdts(args, param_metaffi_types_vec.data(), params_count, retval_count);
		if(pcdts == nullptr)
		{
			Py_INCREF(pPy_None);
			return pPy_None;
		}

		char* out_err = nullptr;
		// xcall always receives 2-element cdts array: [0]=params, [1]=returns
		pxcall(pcdts, &out_err);

		if(out_err)
		{
			pPyErr_SetString(pPyExc_RuntimeError, out_err);
			xllr_free_string(out_err);
			Py_INCREF(pPy_None);
			return pPy_None;
		}

		if(retval_count == 0) {
			Py_INCREF(pPy_None);
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
			// Set the error - PyDLL will automatically check and raise it
			if(pPyExc_RuntimeError != nullptr)
			{
				pPyErr_SetString(pPyExc_RuntimeError, out_err);
			}
			xllr_free_string(out_err);
			Py_INCREF(pPy_None);
			return pPy_None;
		}

		Py_INCREF(pPy_None);
		return pPy_None;
	}
}
