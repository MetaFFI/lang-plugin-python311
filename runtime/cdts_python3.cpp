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

using namespace metaffi::runtime;
std::once_flag load_capi_flag;

const char* create_lambda_python_code = R"(
if 'create_lambda' not in globals():
	import platform
	import os
	import ctypes

	def get_dynamic_lib_path_from_metaffi_home(fname:str):
		osname = platform.system()
		if os.getenv('METAFFI_HOME') is None:
			raise RuntimeError('No METAFFI_HOME environment variable')
		elif fname is None:
			raise RuntimeError('fname is None')

		if osname == 'Windows':
			return os.getenv('METAFFI_HOME') + '\\' + fname + '.dll'
		elif osname == 'Darwin':
			return os.getenv('METAFFI_HOME') + '/' + fname + '.dylib'
		else:
			return os.getenv('METAFFI_HOME') + '/' + fname + '.so'  # for everything that is not windows or mac, return .so

	if platform.system() == 'Windows':
		os.add_dll_directory(os.getenv('METAFFI_HOME')+'\\bin\\')

	xllr_python3 = ctypes.cdll.LoadLibrary(get_dynamic_lib_path_from_metaffi_home('xllr.python311'))
	xllr_python3.call_xcall.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.py_object, ctypes.py_object, ctypes.py_object]
	xllr_python3.call_xcall.restype = ctypes.py_object

	XCallParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallNoParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallNoParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint64))

	def create_lambda(pxcall, context, param_types_without_alias, retval_types_without_alias):
		if len(param_types_without_alias) > 0 and len(retval_types_without_alias) > 0:
			pxcall = XCallParamsRetType(pxcall)
		elif len(param_types_without_alias) > 0 and len(retval_types_without_alias) == 0:
			pxcall = XCallParamsNoRetType(pxcall)
		elif len(param_types_without_alias) == 0 and len(retval_types_without_alias) > 0:
			pxcall = XCallNoParamsRetType(pxcall)
		else:
			pxcall = XCallNoParamsNoRetType(pxcall)

		return lambda *args: xllr_python3.call_xcall(pxcall, context, param_types_without_alias, retval_types_without_alias, None if not args else args)

)";


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
				res.set_item(i, (PyObject*)py_float(cur_cdt->cdt_val.metaffi_float32_val.val));
				break;
				
			case metaffi_float32_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_float32, py_float>(cur_cdt->cdt_val.metaffi_float32_array_val.vals,
				                                                 cur_cdt->cdt_val.metaffi_float32_array_val.dimensions_lengths,
				                                                 cur_cdt->cdt_val.metaffi_float32_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_float64_type:
				res.set_item(i, (PyObject*)py_float(cur_cdt->cdt_val.metaffi_float64_val.val));
				break;
				
			case metaffi_float64_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_float64, py_float>(cur_cdt->cdt_val.metaffi_float64_array_val.vals,
				                                                 cur_cdt->cdt_val.metaffi_float64_array_val.dimensions_lengths,
				                                                 cur_cdt->cdt_val.metaffi_float64_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_int8_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int8_val.val));
				break;
				
			case metaffi_int8_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_int8, py_int>(cur_cdt->cdt_val.metaffi_int8_array_val.vals,
				                                            cur_cdt->cdt_val.metaffi_int8_array_val.dimensions_lengths,
				                                            cur_cdt->cdt_val.metaffi_int8_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint8_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint8_val.val));
				break;
				
			case metaffi_uint8_array_type:
			{
				if(cur_cdt->cdt_val.metaffi_uint8_array_val.dimensions == 1)
				{
					py_bytes b((const char*)cur_cdt->cdt_val.metaffi_uint8_array_val.vals, (Py_ssize_t)cur_cdt->cdt_val.metaffi_uint8_array_val.dimensions_lengths[0]);
					res.set_item(i, b.detach());
				}
				else
				{
					py_list lst;
					lst.add_bytes_array(cur_cdt->cdt_val.metaffi_uint8_array_val.vals,
					                    cur_cdt->cdt_val.metaffi_uint8_array_val.dimensions_lengths,
					                    cur_cdt->cdt_val.metaffi_uint8_array_val.dimensions);
					res.set_item(i, lst.detach());
				}
			}break;
			
			case metaffi_int16_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int16_val.val));
				break;
				
			case metaffi_int16_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_int16, py_int>(cur_cdt->cdt_val.metaffi_int16_array_val.vals,
				                                             cur_cdt->cdt_val.metaffi_int16_array_val.dimensions_lengths,
				                                             cur_cdt->cdt_val.metaffi_int16_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint16_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint16_val.val));
				break;
				
			case metaffi_uint16_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_uint16, py_int>(cur_cdt->cdt_val.metaffi_uint16_array_val.vals,
				                                              cur_cdt->cdt_val.metaffi_uint16_array_val.dimensions_lengths,
				                                              cur_cdt->cdt_val.metaffi_uint16_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_int32_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int32_val.val));
				break;
				
			case metaffi_int32_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_int32, py_int>(cur_cdt->cdt_val.metaffi_int32_array_val.vals,
				                                             cur_cdt->cdt_val.metaffi_int32_array_val.dimensions_lengths,
				                                             cur_cdt->cdt_val.metaffi_int32_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint32_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint32_val.val));
				break;
				
			case metaffi_uint32_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_uint32, py_int>(cur_cdt->cdt_val.metaffi_uint32_array_val.vals,
				                                              cur_cdt->cdt_val.metaffi_uint32_array_val.dimensions_lengths,
				                                              cur_cdt->cdt_val.metaffi_uint32_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_int64_type:
				res.set_item(i, (PyObject*)py_int((int64_t)cur_cdt->cdt_val.metaffi_int64_val.val));
				break;
				
			case metaffi_int64_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_int64, py_int>(cur_cdt->cdt_val.metaffi_int64_array_val.vals,
				                                             cur_cdt->cdt_val.metaffi_int64_array_val.dimensions_lengths,
				                                             cur_cdt->cdt_val.metaffi_int64_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_uint64_type:
				res.set_item(i, (PyObject*)py_int((uint64_t)cur_cdt->cdt_val.metaffi_uint64_val.val));
				break;
				
			case metaffi_uint64_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_uint64, py_int>(cur_cdt->cdt_val.metaffi_uint64_array_val.vals,
				                                              cur_cdt->cdt_val.metaffi_uint64_array_val.dimensions_lengths,
				                                              cur_cdt->cdt_val.metaffi_uint64_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_bool_type:
				res.set_item(i, (PyObject*)py_bool(cur_cdt->cdt_val.metaffi_bool_val.val));
				break;
				
			case metaffi_bool_array_type:
			{
				py_list lst;
				lst.add_numeric_array<metaffi_bool, py_bool>(cur_cdt->cdt_val.metaffi_bool_array_val.vals,
				                                             cur_cdt->cdt_val.metaffi_bool_array_val.dimensions_lengths,
				                                             cur_cdt->cdt_val.metaffi_bool_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_string8_type:
			{
				res.set_item(i, py_str(reinterpret_cast<const char*>(cur_cdt->cdt_val.metaffi_string8_val.val), (int64_t)cur_cdt->cdt_val.metaffi_string8_val.length).detach());
			}break;
			
			case metaffi_string8_array_type:
			{
				py_list lst;
				lst.add_string_array<metaffi_string8, py_str>(cur_cdt->cdt_val.metaffi_string8_array_val.vals,
				                                              cur_cdt->cdt_val.metaffi_string8_array_val.vals_sizes,
				                                              cur_cdt->cdt_val.metaffi_string8_array_val.dimensions_lengths,
															  cur_cdt->cdt_val.metaffi_string8_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_string16_type:
				res.set_item(i, (PyObject*)py_str(reinterpret_cast<const char16_t*>(cur_cdt->cdt_val.metaffi_string16_val.val), (int64_t)cur_cdt->cdt_val.metaffi_string16_val.length).detach());
				break;
				
			case metaffi_string16_array_type:
			{
				py_list lst;
				lst.add_string_array<metaffi_string16, py_str>(cur_cdt->cdt_val.metaffi_string16_array_val.vals,
				                                               cur_cdt->cdt_val.metaffi_string16_array_val.vals_sizes,
				                                               cur_cdt->cdt_val.metaffi_string16_array_val.dimensions_lengths,
				                                               cur_cdt->cdt_val.metaffi_string16_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
			case metaffi_string32_type:
				res.set_item(i, (PyObject*)py_str(reinterpret_cast<const char32_t*>(cur_cdt->cdt_val.metaffi_string32_val.val), (int64_t)cur_cdt->cdt_val.metaffi_string32_val.length).detach());
				break;
				
			case metaffi_string32_array_type:
			{
				py_list lst;
				lst.add_string_array<metaffi_string32, py_str>(cur_cdt->cdt_val.metaffi_string32_array_val.vals,
				                                               cur_cdt->cdt_val.metaffi_string32_array_val.vals_sizes,
				                                               cur_cdt->cdt_val.metaffi_string32_array_val.dimensions_lengths,
				                                               cur_cdt->cdt_val.metaffi_string32_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
				
			case metaffi_handle_type:
			{
				res.set_item(i, py_metaffi_handle::extract_pyobject_from_handle(&cur_cdt->cdt_val.metaffi_handle_val));
			}break;
			
			case metaffi_handle_array_type:
			{
				py_list lst;
				lst.add_handle_array(cur_cdt->cdt_val.metaffi_handle_array_val.vals,
				                     cur_cdt->cdt_val.metaffi_handle_array_val.dimensions_lengths,
				                     cur_cdt->cdt_val.metaffi_handle_array_val.dimensions);
				res.set_item(i, lst.detach());
			}break;
			
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
	py_tuple pyobjs = expected_types_length <= 1 ? py_tuple(&pyobject_or_tuple, 1) : py_tuple(pyobject_or_tuple);
	pyobjs.inc_ref(); // prevent py_tuple from releasing the object
	
	if(pyobjs.length() != expected_types_length)
	{
		throw std::runtime_error("tuple and tuple_types have different lengths");
	}
	
	for(int i = 0 ; i < expected_types_length ; i++)
	{
		metaffi_type_info& type = expected_types[i];
		cdts[i]->type = expected_types[i].type;
		
		if(type.type == metaffi_any_type)
		{
			type = py_object(pyobjs[i]).get_type_info();
			cdts[i]->type = type.type;
		}
		
		switch(type.type)
		{
			case metaffi_float32_type:
				cdts[i]->cdt_val.metaffi_float32_val.val = (metaffi_float32)py_float(pyobjs[i]);
				break;
				
			case metaffi_float32_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_float32_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_float32, py_float>(&cdts[i]->cdt_val.metaffi_float32_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_float32_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_float64_type:
				cdts[i]->cdt_val.metaffi_float64_val.val = (metaffi_float64)py_float(pyobjs[i]);
				break;
				
			case metaffi_float64_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_float64_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_float64, py_float>(&cdts[i]->cdt_val.metaffi_float64_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_float64_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_int8_type:
				cdts[i]->cdt_val.metaffi_int8_val.val = (metaffi_int8)py_int(pyobjs[i]);
				break;
				
			case metaffi_int8_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_int8_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_int8, py_int>(&cdts[i]->cdt_val.metaffi_int8_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_int8_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_uint8_type:
				cdts[i]->cdt_val.metaffi_uint8_val.val = (metaffi_uint8)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint8_array_type:
			{
				if(type.dimensions == 1 && py_bytes::check(pyobjs[i]))
				{
					py_bytes b(pyobjs[i]);
					
					cdts[i]->cdt_val.metaffi_uint8_array_val.dimensions = 1;
					cdts[i]->cdt_val.metaffi_uint8_array_val.vals = (uint8_t*)b;
					cdts[i]->cdt_val.metaffi_uint8_array_val.dimensions_lengths = new metaffi_size[1];
					cdts[i]->cdt_val.metaffi_uint8_array_val.dimensions_lengths[0] = b.size();
				}
				else
				{
					py_list lst(pyobjs[i]);
					cdts[i]->cdt_val.metaffi_uint8_array_val.dimensions = type.dimensions;
					lst.get_bytes_array(
							&cdts[i]->cdt_val.metaffi_uint8_array_val.vals,
							&cdts[i]->cdt_val.metaffi_uint8_array_val.dimensions_lengths,
					                                               type.dimensions);
				}
			}break;
			
			case metaffi_int16_type:
				cdts[i]->cdt_val.metaffi_int16_val.val = (metaffi_int16)py_int(pyobjs[i]);
				break;
				
			case metaffi_int16_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_int16_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_int16, py_int>(&cdts[i]->cdt_val.metaffi_int16_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_int16_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_uint16_type:
				cdts[i]->cdt_val.metaffi_uint16_val.val = (metaffi_uint16)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint16_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_uint16_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_uint16, py_int>(&cdts[i]->cdt_val.metaffi_uint16_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_uint16_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_int32_type:
				cdts[i]->cdt_val.metaffi_int32_val.val = (metaffi_int32)py_int(pyobjs[i]);
				break;
				
			case metaffi_int32_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_int32_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_int32, py_int>(&cdts[i]->cdt_val.metaffi_int32_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_int32_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_uint32_type:
				cdts[i]->cdt_val.metaffi_uint32_val.val = (metaffi_uint32)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint32_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_uint32_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_uint32, py_int>(&cdts[i]->cdt_val.metaffi_uint32_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_uint32_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_int64_type:
				cdts[i]->cdt_val.metaffi_int64_val.val = (metaffi_int64)py_int(pyobjs[i]);
				break;
				
			case metaffi_int64_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_int64_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_int64, py_int>(&cdts[i]->cdt_val.metaffi_int64_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_int64_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
			
			case metaffi_uint64_type:
				cdts[i]->cdt_val.metaffi_uint64_val.val = (metaffi_uint64)py_int(pyobjs[i]);
				break;
				
			case metaffi_uint64_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_uint64_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_uint64, py_int>(&cdts[i]->cdt_val.metaffi_uint64_array_val.vals,
				                                                 &cdts[i]->cdt_val.metaffi_uint64_array_val.dimensions_lengths,
				                                                 type.dimensions);
			}break;
				
			case metaffi_bool_type:
				cdts[i]->cdt_val.metaffi_bool_val.val = (metaffi_bool)py_bool(pyobjs[i]);
				break;
				
			case metaffi_bool_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_bool_array_val.dimensions = type.dimensions;
				lst.get_numeric_array<metaffi_bool, py_bool>(&cdts[i]->cdt_val.metaffi_bool_array_val.vals,
				                                             &cdts[i]->cdt_val.metaffi_bool_array_val.dimensions_lengths,
				                                             type.dimensions);
			}break;
				
			case metaffi_string8_type:
			{
				py_str str(pyobjs[i]);
				std::string string = str.to_utf8();
				cdts[i]->cdt_val.metaffi_string8_val.val = new metaffi_char8[string.size()];
				cdts[i]->cdt_val.metaffi_string8_val.length = string.size();
				std::copy(string.begin(), string.end(), cdts[i]->cdt_val.metaffi_string8_val.val);
			}break;
			
			case metaffi_string8_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_string8_array_val.dimensions = type.dimensions;
				lst.get_string_array<metaffi_string8, py_str, char>(&cdts[i]->cdt_val.metaffi_string8_array_val.vals,
				                                                    &cdts[i]->cdt_val.metaffi_string8_array_val.vals_sizes,
				                                                    &cdts[i]->cdt_val.metaffi_string8_array_val.dimensions_lengths,
				                                                    type.dimensions);
			}break;
			
			case metaffi_string16_type:
			{
				py_str str(pyobjs[i]);
				std::u16string string = str.to_utf16();
				cdts[i]->cdt_val.metaffi_string16_val.val = new metaffi_char16[string.size()];
				cdts[i]->cdt_val.metaffi_string16_val.length = string.size();
				std::copy(string.begin(), string.end(), cdts[i]->cdt_val.metaffi_string16_val.val);
			}break;
			
			case metaffi_string16_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_string16_array_val.dimensions = type.dimensions;
				lst.get_string_array<metaffi_string16, py_str, char16_t>(&cdts[i]->cdt_val.metaffi_string16_array_val.vals,
				                                                    &cdts[i]->cdt_val.metaffi_string16_array_val.vals_sizes,
				                                                    &cdts[i]->cdt_val.metaffi_string16_array_val.dimensions_lengths,
				                                                    type.dimensions);
			}break;
			
			case metaffi_string32_type:
			{
				py_str str(pyobjs[i]);
				std::u32string string = str.to_utf32();
				cdts[i]->cdt_val.metaffi_string32_val.val = new metaffi_char32[string.size()];
				cdts[i]->cdt_val.metaffi_string32_val.length = string.size();
				std::copy(string.begin(), string.end(), cdts[i]->cdt_val.metaffi_string32_val.val);
			}break;
			
			case metaffi_string32_array_type:
			{
				py_list lst(pyobjs[i]);
				cdts[i]->cdt_val.metaffi_string32_array_val.dimensions = type.dimensions;
				lst.get_string_array<metaffi_string32, py_str, char32_t>(&cdts[i]->cdt_val.metaffi_string32_array_val.vals,
				                                                    &cdts[i]->cdt_val.metaffi_string32_array_val.vals_sizes,
				                                                    &cdts[i]->cdt_val.metaffi_string32_array_val.dimensions_lengths,
				                                                    type.dimensions);
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
				cdts[i]->cdt_val.metaffi_handle_array_val.dimensions = type.dimensions;
				lst.get_handle_array(&cdts[i]->cdt_val.metaffi_handle_array_val.vals,
				                     &cdts[i]->cdt_val.metaffi_handle_array_val.dimensions_lengths,
				                     type.dimensions);
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