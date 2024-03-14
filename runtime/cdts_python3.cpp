#include "cdts_python3.h"
#include "objects_table.h"
#include "py_metaffi_handle.h"
#include <runtime/cdt_capi_loader.h>
#include <mutex>
#include "runtime_id.h"
#include "utils.h"
#include "py_tuple.h"
#include "py_float.h"
#include "py_str.h"
#include "py_bool.h"
#include "py_int.h"
#include "py_list.h"
#include "py_metaffi_handle.h"
#include "py_int.h"
#include "py_bytes.h"
#include "py_metaffi_callable.h"

using namespace metaffi::runtime;
std::once_flag load_capi_flag;


//	[](void* values_to_set, int index, const cdt_metaffi_callable& val_to_set)
//	{
//		PyRun_SimpleString(create_lambda_python_code); // make sure "create_lambda" exists
//		PyObject* main_module = PyImport_AddModule("__main__");  // Get the main module
//		PyObject* global_dict = PyModule_GetDict(main_module);  // Get the global dictionary
//
//		// Get the create_lambda function
//		PyObject* pyFunc = PyDict_GetItemString(global_dict, "create_lambda");
//
//		if(!pyFunc || !PyCallable_Check(pyFunc))
//		{
//			throw std::runtime_error("failed to create or import create_lambda python function");
//		}
//
//		// Convert the void* to PyObject*
//		PyObject* py_pxcall = PyLong_FromVoidPtr(((void**)val_to_set.val)[0]);
//		PyObject* py_pcontext = PyLong_FromVoidPtr(((void**)val_to_set.val)[1]);
//
//		// Convert the metaffi_type arrays to PyTuple*
//		PyObject* py_param_types = PyTuple_New(val_to_set.params_types_length);
//		for (int i = 0; i < val_to_set.params_types_length; i++) {
//			PyTuple_SetItem(py_param_types, i, PyLong_FromUnsignedLongLong(val_to_set.parameters_types[i]));
//		}
//
//		PyObject* py_retval_types = PyTuple_New(val_to_set.retval_types_length);
//		for (int i = 0; i < val_to_set.retval_types_length; i++) {
//			PyTuple_SetItem(py_retval_types, i, PyLong_FromUnsignedLongLong(val_to_set.retval_types[i]));
//		}
//
//		PyObject* argsTuple = PyTuple_New(4);  // Create a tuple to hold the arguments
//
//		PyTuple_SetItem(argsTuple, 0, py_pxcall);
//		PyTuple_SetItem(argsTuple, 1, py_pcontext);
//		PyTuple_SetItem(argsTuple, 2, py_param_types);
//		PyTuple_SetItem(argsTuple, 3, py_retval_types);
//
//		// Call the Python function
//		PyObject* result = PyObject_CallObject(pyFunc, argsTuple);
//		Py_DECREF(argsTuple);
//
//		std::string err = check_python_error();
//		if(!err.empty())
//		{
//			throw std::runtime_error(err);
//		}
//
//		PyTuple_SetItem((PyObject*)values_to_set, index, result); // set lambda in return values tuple
//	}

auto get_on_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_size, metaffi_size array_length, void* other_array)
	{
		// create the array in the given indices
		py_list lst = *(py_list*)other_array;
		for(int i=0 ; i<index_size ; i++)
		{
			lst = lst[index[i]];
		}
		
		for(int i=0 ; i<array_length ; i++)
		{
			lst.append(py_list().detach());
		}
	};
}

template<typename metaffi_type_t, typename py_type_t>
auto get_on_1d_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_size, metaffi_type_t* arr, metaffi_size length, void* other_array)
	{
		// copy the 1D array to python
		py_list lst = *(py_list*)other_array;
		for(int i=0 ; i<index_size ; i++)
		{
			lst = lst[index[i]];
		}
		
		for(int i=0 ; i<length ; i++)
		{
			lst.append((PyObject*)(py_type_t(arr[i]).detach()));
		}
	};
}

auto get_on_1d_handle_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_size, cdt_metaffi_handle* arr, metaffi_size length, void* other_array)
	{
		// copy the 1D array to python
		py_list lst = *(py_list*)other_array;
		for(int i=0 ; i<index_size ; i++)
		{
			lst = lst[index[i]];
		}
		
		for(int i=0 ; i<length ; i++)
		{
			lst.append((PyObject*)(py_metaffi_handle::extract_pyobject_from_handle(arr[i]).detach()));
		}
	};
}

auto get_on_bytes_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_size, metaffi_size array_length, void* other_array)
	{
		// create the array in the given indices
		py_list lst = *(py_list*)other_array;
		for(int i=0 ; i<index_size ; i++)
		{
			lst = lst[index[i]];
		}
		
		for(int i=0 ; i<array_length && index_size > 1; i++)
		{
			lst.append(py_list().detach());
		}
	};
}

auto get_on_1d_bytes_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_size, metaffi_uint8* arr, metaffi_size length, void* other_array)
	{
		// copy the 1D array to python
		py_list lst = *(py_list*)other_array;
		for(int i=0 ; i<((int64_t)index_size)-1 ; i++)
		{
			lst = lst[index[i]];
		}
		
		lst.append((PyObject*)(py_bytes((const char*)arr, (Py_ssize_t)length).detach()));
	};
}

auto get_get_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, void* other_array)->metaffi_size
	{
		py_list lst = *(py_list*)other_array;
		for (metaffi_size i = 0; i < index_length; i++)
		{
			lst = lst[index[i]];
		}
		
		return lst.length();
	};
}

template<typename metaffi_type_t, typename py_type_t>
auto get_get_1d_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, metaffi_size& out_1d_array_length, void* other_array)->metaffi_type_t*
	{
		py_list lst = *(py_list*)other_array;
		for (metaffi_size i = 0; i < index_length; i++)
		{
			lst = lst[index[i]];
		}
		
		out_1d_array_length = lst.length();
		
		metaffi_type_t* arr = new metaffi_type_t[out_1d_array_length]{};
		for(int i=0 ; i<out_1d_array_length ; i++)
		{
			arr[i] = (metaffi_type_t)py_type_t(lst[i]);
		}
		
		return arr;
	};
}

auto get_get_1d_string8_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, metaffi_size& out_1d_array_length, void* other_array)->metaffi_string8*
	{
		py_list lst = *(py_list*)other_array;
		for (metaffi_size i = 0; i < index_length; i++)
		{
			lst = lst[index[i]];
		}
		
		out_1d_array_length = lst.length();
		metaffi_string8* arr = new metaffi_string8[out_1d_array_length]{};
		
		for(int i=0 ; i<out_1d_array_length ; i++)
		{
			py_str str(lst[i]);
			std::string string = str.to_utf8();
			metaffi_char8* pstr = new metaffi_char8[string.size()+1]{};
			std::copy(string.begin(), string.end(), pstr);
			arr[i] = pstr;
		}
		
		return arr;
	};
}

auto get_get_1d_string16_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, metaffi_size& out_1d_array_length, void* other_array)->metaffi_string16*
	{
		py_list lst = *(py_list*)other_array;
		for (metaffi_size i = 0; i < index_length; i++)
		{
			lst = lst[index[i]];
		}
		
		out_1d_array_length = lst.length();
		metaffi_string16* arr = new metaffi_string16[out_1d_array_length]{};
		
		for(int i=0 ; i<out_1d_array_length ; i++)
		{
			py_str str(lst[i]);
			std::basic_string<metaffi_char16> string = str.to_utf16();
			metaffi_char16* pstr = new metaffi_char16[string.size()+1]{};
			std::copy(string.begin(), string.end(), pstr);
			arr[i] = pstr;
		}
		
		return arr;
	};
}

auto get_get_1d_string32_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, metaffi_size& out_1d_array_length, void* other_array)->metaffi_string32*
	{
		py_list lst = *(py_list*)other_array;
		for (metaffi_size i = 0; i < index_length; i++)
		{
			lst = lst[index[i]];
		}
		
		out_1d_array_length = lst.length();
		metaffi_string32* arr = new metaffi_string32[out_1d_array_length]{};
		
		for(int i=0 ; i<out_1d_array_length ; i++)
		{
			py_str str(lst[i]);
			std::basic_string<metaffi_char32> string = str.to_utf32();
			metaffi_char32* pstr = new metaffi_char32[string.size()+1]{};
			std::copy(string.begin(), string.end(), pstr);
			arr[i] = pstr;
		}
		
		return arr;
	};
}

auto get_get_1d_handle_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, metaffi_size& out_1d_array_length, void* other_array)->cdt_metaffi_handle*
	{
		py_list lst = *(py_list*)other_array;
		for (metaffi_size i = 0; i < index_length; i++)
		{
			lst = lst[index[i]];
		}
		
		out_1d_array_length = lst.length();
		cdt_metaffi_handle* arr = new cdt_metaffi_handle[out_1d_array_length]{};
		
		for(int i=0 ; i<out_1d_array_length ; i++)
		{
			PyObject* elem = lst[i];
			
			if(elem == Py_None)
			{
				arr[i] = {nullptr, 0, nullptr};
			}
			else if(py_metaffi_handle::check(elem))
			{
				arr[i] = (cdt_metaffi_handle)py_metaffi_handle(elem);
			}
			else // different object - wrap in metaffi_handle
			{
				arr[i] = { elem, PYTHON311_RUNTIME_ID, nullptr};
			}
		}
		
		return arr;
	};
}

auto get_get_bytes_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, void* other_array)->metaffi_size
	{
		py_list lst((PyObject*)other_array);
		lst.inc_ref();
		for (metaffi_size i = 0; i < index_length; i++)
		{
			lst = lst[index[i]];
		}
		
		return lst.length();
	};
}

auto get_get_1d_bytes_array_callback()
{
	return [](metaffi_size* index, metaffi_size index_length, metaffi_size& out_1d_array_length, void* other_array)->metaffi_uint8*
	{
		PyObject* cur = (PyObject*)other_array;
		
		if(py_bytes::check(cur))
		{
			py_bytes bytes(cur);
			out_1d_array_length = bytes.size();
			return (metaffi_uint8*)bytes;
		}
		else if(py_list::check(cur))
		{
			py_list lst((PyObject*)other_array);
			lst.inc_ref();
			for (metaffi_size i = 0; i < index_length-1; i++)
			{
				lst = lst[index[i]];
			}
			
			py_bytes bytes(lst[index[index_length-1]]);
			out_1d_array_length = bytes.size();
			return (metaffi_uint8*)bytes;
		}
		else
		{
			std::stringstream ss;
			ss << "Bytes array(s) expecting list or bytes. Received: " << cur->ob_type->tp_name;
			throw std::runtime_error(ss.str());
		}
	};
}

//--------------------------------------------------------------------
cdts_python3::cdts_python3(cdt* cdts, metaffi_size cdts_length): cdts(cdts, cdts_length)
{
}
//--------------------------------------------------------------------
py_tuple cdts_python3::to_py_tuple()
{
	int cdts_length = this->cdts.get_cdts_length();
	
	py_tuple res((Py_ssize_t)cdts_length);
	
	for(int i=0 ; i<cdts_length ; i++)
	{
		cdt* cur_cdt = cdts[i];
		
		switch(cur_cdt->type)
		{
			case metaffi_float32_type:
				res.set_item(i, (PyObject*)py_float(cur_cdt->cdt_val.metaffi_float32_val).detach());
				break;
				
			case metaffi_float32_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_float32_array, metaffi_float32>(
					cur_cdt->cdt_val.metaffi_float32_array_val,
					&lst,
					get_on_array_callback(),
					get_on_1d_array_callback<metaffi_float32, py_float>()
				);
				
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_float64_type:
				res.set_item(i, (PyObject*)py_float(cur_cdt->cdt_val.metaffi_float64_val).detach());
				break;
				
			case metaffi_float64_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_float64_array, metaffi_float64>(
						cur_cdt->cdt_val.metaffi_float64_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_float64, py_float>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_int8_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int8_val).detach());
				break;
				
			case metaffi_int8_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_int8_array, metaffi_int8>(
						cur_cdt->cdt_val.metaffi_int8_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_int8, py_int>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint8_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint8_val).detach());
				break;
				
			case metaffi_uint8_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_uint8_array, metaffi_uint8>(
						cur_cdt->cdt_val.metaffi_uint8_array_val,
						&lst,
						get_on_bytes_array_callback(),
						get_on_1d_bytes_array_callback()
				);
				
				if(cur_cdt->cdt_val.metaffi_uint8_array_val.dimension == 1)
				{
					res.set_item(i, lst[0]);
				}
				else
				{
					res.set_item(i, lst.detach());
				}
				
			}break;
			
			case metaffi_int16_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int16_val).detach());
				break;
				
			case metaffi_int16_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_int16_array, metaffi_int16>(
						cur_cdt->cdt_val.metaffi_int16_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_int16, py_int>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint16_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint16_val).detach());
				break;
				
			case metaffi_uint16_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_uint16_array, metaffi_uint16>(
						cur_cdt->cdt_val.metaffi_uint16_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_uint16, py_int>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_int32_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int32_val).detach());
				break;
				
			case metaffi_int32_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_int32_array, metaffi_int32>(
						cur_cdt->cdt_val.metaffi_int32_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_int32, py_int>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint32_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint32_val).detach());
				break;
				
			case metaffi_uint32_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_uint32_array, metaffi_uint32>(
						cur_cdt->cdt_val.metaffi_uint32_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_uint32, py_int>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_int64_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int64_val).detach());
				break;
				
			case metaffi_int64_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_int64_array, metaffi_int64>(
						cur_cdt->cdt_val.metaffi_int64_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_int64, py_int>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint64_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint64_val).detach());
				break;
				
			case metaffi_uint64_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_uint64_array, metaffi_uint64>(
						cur_cdt->cdt_val.metaffi_uint64_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_uint64, py_int>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_bool_type:
				res.set_item(i, (PyObject*)py_bool(cur_cdt->cdt_val.metaffi_bool_val).detach());
				break;
				
			case metaffi_bool_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_bool_array, metaffi_bool>(
						cur_cdt->cdt_val.metaffi_bool_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_bool, py_bool>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_string8_type:
			{
				res.set_item(i, py_str(cur_cdt->cdt_val.metaffi_string8_val).detach());
			}break;
			
			case metaffi_string8_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_string8_array, metaffi_string8>(
						cur_cdt->cdt_val.metaffi_string8_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_string8, py_str>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_string16_type:
				res.set_item(i, (PyObject*)py_str(cur_cdt->cdt_val.metaffi_string16_val).detach());
				break;
				
			case metaffi_string16_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_string16_array, metaffi_string16>(
						cur_cdt->cdt_val.metaffi_string16_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_string16, py_str>()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_string32_type:
				res.set_item(i, (PyObject*)py_str(cur_cdt->cdt_val.metaffi_string32_val).detach());
				break;
				
			case metaffi_string32_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_string32_array, metaffi_string32>(
						cur_cdt->cdt_val.metaffi_string32_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_array_callback<metaffi_string32, py_str>()
				);
				res.set_item(i, lst.detach());
			}break;
				
			case metaffi_handle_type:
			{
				res.set_item(i, py_metaffi_handle::extract_pyobject_from_handle(cur_cdt->cdt_val.metaffi_handle_val).detach());
			}break;
			
			case metaffi_handle_array_type:
			{
				py_list lst;
				metaffi::runtime::traverse_multidim_array<cdt_metaffi_handle_array, cdt_metaffi_handle>(
						cur_cdt->cdt_val.metaffi_handle_array_val,
						&lst,
						get_on_array_callback(),
						get_on_1d_handle_array_callback()
				);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_callable_type:
			{
				py_metaffi_callable callable(cur_cdt->cdt_val.metaffi_callable_val);
				res.set_item(i, callable.detach()); // set lambda in return values tuple
			}
			
			default:
				std::stringstream ss;
				ss << "Unknown type " << cur_cdt->type << " at index " << i;
				throw std::runtime_error(ss.str());
		}
	}
	
	return res;
}
//--------------------------------------------------------------------
void cdts_python3::to_cdts(PyObject* pyobject_or_tuple, metaffi_type_info* expected_types, int expected_types_length)
{
	py_tuple pyobjs = expected_types_length <= 1 && !PyTuple_Check(pyobject_or_tuple) ? py_tuple(&pyobject_or_tuple, 1) : py_tuple(pyobject_or_tuple);
	pyobjs.inc_ref(); // TODO: (is it correct?) prevent py_tuple from releasing the object
	
	if(pyobjs.length() != expected_types_length)
	{
		throw std::runtime_error("tuple and tuple_types have different lengths");
	}
	
	for(int i = 0 ; i < expected_types_length ; i++)
	{
		metaffi_type_info type = expected_types[i];
		cdts[i]->type = expected_types[i].type;
		
		if(type.type == metaffi_any_type)
		{
			type = py_object(pyobjs[i]).get_type_info();
			cdts[i]->type = type.type;
		}
		
		switch(type.type)
		{
			case metaffi_float32_type:
				cdts[i]->cdt_val.metaffi_float32_val = (metaffi_float32)py_float(pyobjs[i]);
				break;
				
			case metaffi_float32_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_float32_array, metaffi_float32>(
						cdts[i]->cdt_val.metaffi_float32_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_float32, py_float>());
			}break;
			
			case metaffi_float64_type:
				cdts[i]->cdt_val.metaffi_float64_val = (metaffi_float64)py_float(pyobjs[i]);
				break;
				
			case metaffi_float64_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_float64_array, metaffi_float64>(
						cdts[i]->cdt_val.metaffi_float64_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_float64, py_float>());
			}break;
			
			case metaffi_int8_type:
				cdts[i]->cdt_val.metaffi_int8_val = (metaffi_int8)py_int(pyobjs[i]);
				break;
				
			case metaffi_int8_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_int8_array, metaffi_int8>(
						cdts[i]->cdt_val.metaffi_int8_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_int8, py_int>());
			}break;
			
			case metaffi_uint8_type:
				cdts[i]->cdt_val.metaffi_uint8_val = (metaffi_uint8)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint8_array_type:
			{
				PyObject* obj = pyobjs[i]; // can be either list or bytes
				metaffi::runtime::construct_multidim_array<cdt_metaffi_uint8_array, metaffi_uint8>(
						cdts[i]->cdt_val.metaffi_uint8_array_val,
						type.dimensions,
						obj,
						get_get_bytes_array_callback(),
						get_get_1d_bytes_array_callback());
			}break;
			
			case metaffi_int16_type:
				cdts[i]->cdt_val.metaffi_int16_val = (metaffi_int16)py_int(pyobjs[i]);
				break;
				
			case metaffi_int16_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_int16_array, metaffi_int16>(
						cdts[i]->cdt_val.metaffi_int16_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_int16, py_int>());
			}break;
			
			case metaffi_uint16_type:
				cdts[i]->cdt_val.metaffi_uint16_val = (metaffi_uint16)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint16_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_uint16_array, metaffi_uint16>(
						cdts[i]->cdt_val.metaffi_uint16_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_uint16, py_int>());
			}break;
			
			case metaffi_int32_type:
				cdts[i]->cdt_val.metaffi_int32_val = (metaffi_int32)py_int(pyobjs[i]);
				break;
				
			case metaffi_int32_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_int32_array, metaffi_int32>(
						cdts[i]->cdt_val.metaffi_int32_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_int32, py_int>());
			}break;
			
			case metaffi_uint32_type:
				cdts[i]->cdt_val.metaffi_uint32_val = (metaffi_uint32)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint32_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_uint32_array, metaffi_uint32>(
						cdts[i]->cdt_val.metaffi_uint32_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_uint32, py_int>());
			}break;
			
			case metaffi_int64_type:
				cdts[i]->cdt_val.metaffi_int64_val = (metaffi_int64)py_int(pyobjs[i]);
				break;
				
			case metaffi_int64_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_int64_array, metaffi_int64>(
						cdts[i]->cdt_val.metaffi_int64_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_int64, py_int>());
			}break;
			
			case metaffi_uint64_type:
				cdts[i]->cdt_val.metaffi_uint64_val = (metaffi_uint64)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint64_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_uint64_array, metaffi_uint64>(
						cdts[i]->cdt_val.metaffi_uint64_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_uint64, py_int>());
			}break;
				
			case metaffi_bool_type:
				cdts[i]->cdt_val.metaffi_bool_val = (metaffi_bool)py_bool(pyobjs[i]);
				break;
				
			case metaffi_bool_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_bool_array, metaffi_bool>(
						cdts[i]->cdt_val.metaffi_bool_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_array_callback<metaffi_bool, py_bool>());
			}break;
				
			case metaffi_string8_type:
			{
				py_str str(pyobjs[i]);
				std::string string = str.to_utf8();
				cdts[i]->cdt_val.metaffi_string8_val = new metaffi_char8[string.size()+1]{};
				std::copy(string.begin(), string.end(), cdts[i]->cdt_val.metaffi_string8_val);
			}break;
			
			case metaffi_string8_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_string8_array, metaffi_string8>(
						cdts[i]->cdt_val.metaffi_string8_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_string8_array_callback());
			}break;
			
			case metaffi_string16_type:
			{
				py_str str(pyobjs[i]);
				std::u16string string = str.to_utf16();
				cdts[i]->cdt_val.metaffi_string16_val = new metaffi_char16[string.size()+1]{};
				std::copy(string.begin(), string.end(), cdts[i]->cdt_val.metaffi_string16_val);
			}break;
			
			case metaffi_string16_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_string16_array, metaffi_string16>(
						cdts[i]->cdt_val.metaffi_string16_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_string16_array_callback());
			}break;
			
			case metaffi_string32_type:
			{
				py_str str(pyobjs[i]);
				std::u32string string = str.to_utf32();
				cdts[i]->cdt_val.metaffi_string32_val = new metaffi_char32[string.size()+1];
				std::copy(string.begin(), string.end(), cdts[i]->cdt_val.metaffi_string32_val);
			}break;
			
			case metaffi_string32_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_string32_array, metaffi_string32>(
						cdts[i]->cdt_val.metaffi_string32_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_string32_array_callback());
			}break;
			
			case metaffi_handle_type:
			{
				if(pyobjs[i] == Py_None)
				{
					cdts.set_null_handle(i);
				}
				else if(py_metaffi_handle::check(pyobjs[i]))
				{
					cdt_metaffi_handle h = (cdt_metaffi_handle)py_metaffi_handle(pyobjs[i]);
					cdts.set(i, h);
				}
				else // different object - wrap in metaffi_handle
				{
					py_object obj(pyobjs[i]);
					cdts[i]->cdt_val.metaffi_handle_val = { obj.detach(), PYTHON311_RUNTIME_ID, nullptr};
				}
			}break;
			
			case metaffi_handle_array_type:
			{
				py_list lst(pyobjs[i]);
				metaffi::runtime::construct_multidim_array<cdt_metaffi_handle_array, cdt_metaffi_handle>(
						cdts[i]->cdt_val.metaffi_handle_array_val,
						type.dimensions,
						&lst,
						get_get_array_callback(),
						get_get_1d_handle_array_callback());
			}break;
			
			default:
			{
				std::stringstream ss;
				ss << "Unknown type " << type.type << " at index " << i;
				throw std::runtime_error(ss.str());
			}break;
		}
	}
}
//--------------------------------------------------------------------