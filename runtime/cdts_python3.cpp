#include "cdts_python3.h"
#include "objects_table.h"
#include "py_metaffi_handle.h"
#include <runtime/cdt_capi_loader.h>
#include <mutex>
#include "runtime_id.h"
#include "python_error.h"

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


	os.add_dll_directory(os.getenv('METAFFI_HOME')+'\\bin\\')

	xllr_python3 = ctypes.cdll.LoadLibrary(get_dynamic_lib_path_from_metaffi_home('xllr.python3'))
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


cdts_build_callbacks cdts_python3::build_callback
{
	[](void* values_to_set, int index, metaffi_float32& val, int starting_index) {
		set_numeric_to_cdts<metaffi_float32>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_float32{ return (metaffi_float32)PyFloat_AsDouble(o); }, [](PyObject* o)->int{ return (int)PyFloat_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_float32*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_float32>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_float32{ return (metaffi_float32)PyFloat_AsDouble(o); }, [](PyObject* o)->int{ return (int)PyFloat_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_float64& val, int starting_index){
		set_numeric_to_cdts<metaffi_float64>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_float64{ return (metaffi_float64)PyFloat_AsDouble(o); }, [](PyObject* o)->int{ return (int)PyFloat_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_float64*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_float64>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_float64{ return (metaffi_float64)PyFloat_AsDouble(o); }, [](PyObject* o)->int{ return (int)PyFloat_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_int8& val, int starting_index){
		set_numeric_to_cdts<metaffi_int8>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_int8{ return (metaffi_int8)PyLong_AsLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_int8*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_int8>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_int8{ return (metaffi_int8)PyLong_AsLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_int16& val, int starting_index){
		set_numeric_to_cdts<metaffi_int16>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_int16{ return (metaffi_int16)PyLong_AsLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_int16*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_int16>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_int16{ return (metaffi_int16)PyLong_AsLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_int32& val, int starting_index) {
		set_numeric_to_cdts<metaffi_int32>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_int32{ return (metaffi_int32)PyLong_AsLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_int32*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_int32>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_int32{ return (metaffi_int32)PyLong_AsLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_int64& val, int starting_index) {
		set_numeric_to_cdts<metaffi_int64>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_int64{ return (metaffi_int64)PyLong_AsLongLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_int64*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_int64>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_int64{ return (metaffi_int64)PyLong_AsLongLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_uint8& val, int starting_index){
		set_numeric_to_cdts<metaffi_uint8>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_uint8{ return (metaffi_uint8)PyLong_AsUnsignedLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_uint8*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_uint8>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_uint8{ return (metaffi_uint8)PyLong_AsUnsignedLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_uint16& val, int starting_index){
		set_numeric_to_cdts<metaffi_uint16>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_uint16{ return (metaffi_uint16)PyLong_AsUnsignedLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_uint16*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_uint16>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_uint16{ return (metaffi_uint16)PyLong_AsUnsignedLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_uint32& val, int starting_index) {
		set_numeric_to_cdts<metaffi_uint32>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_uint32{ return (metaffi_uint32)PyLong_AsUnsignedLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_uint32*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_uint32>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_uint32{ return (metaffi_uint32)PyLong_AsUnsignedLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	
	[](void* values_to_set, int index, metaffi_uint64& val, int starting_index) {
		set_numeric_to_cdts<metaffi_uint64>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* o)->metaffi_uint64{ return (metaffi_uint64)PyLong_AsUnsignedLongLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_uint64*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_uint64>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* o)->metaffi_uint64{ return (metaffi_uint64)PyLong_AsUnsignedLongLong(o); }, [](PyObject* o)->int{ return (int)PyLong_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_bool& val, int starting_index){
		set_numeric_to_cdts<metaffi_bool>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* pybool)->int{ return pybool == Py_False? 0 : 1; }, [](PyObject* o)->int{ return PyBool_Check(o); });
	},
	[](void* values_to_set, int index, metaffi_bool*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_numeric_array_to_cdts<metaffi_bool>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* pybool)->int{ return pybool == Py_False? 0 : 1; }, [](PyObject* o)->int{ return PyBool_Check(o); });
	},
	[](void* values_to_set, int index, cdt_metaffi_handle& val, int starting_index)
	{
		auto set_object = [](PyObject* pybj)->cdt_metaffi_handle
		{
			if(pybj == Py_None){
				return cdt_metaffi_handle{0, PYTHON3_RUNTIME_ID};
			}
			
			if(!python3_objects_table::instance().contains(pybj))
			{
				// if metaffi handle - pass as it is.
				if(strcmp(pybj->ob_type->tp_name, "metaffi_handle") == 0)
				{
					PyObject* pyhandle = PyObject_GetAttrString(pybj, "handle");
					if(!pyhandle)
					{
						throw std::runtime_error("handle attribute is not found in metaffi_handle object");
					}
					
					PyObject* runtime_id = PyObject_GetAttrString(pybj, "runtime_id");
					if(!runtime_id)
					{
						throw std::runtime_error("runtime_id attribute is not found in metaffi_handle object");
					}
					
					auto res = cdt_metaffi_handle{(metaffi_handle)PyLong_AsUnsignedLongLong(pyhandle),
					                              PyLong_AsUnsignedLongLong(runtime_id)};
					
					return res;
				}
				
				// a python object
				python3_objects_table::instance().set(pybj);
			};
			
			return {(metaffi_handle)pybj, PYTHON3_RUNTIME_ID};
		};
		
		set_numeric_to_cdts<cdt_metaffi_handle>((PyObject*)values_to_set, index+starting_index, val, set_object, [](PyObject* o)->int{ return 1; });
	},
	[](void* values_to_set, int index, cdt_metaffi_handle* arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index)
	{
		auto set_object = [](PyObject* pybj)->cdt_metaffi_handle
		{
			if(pybj == Py_None){
				return {nullptr, PYTHON3_RUNTIME_ID};
			}
			
			if(!python3_objects_table::instance().contains(pybj))
			{
				// if metaffi handle - pass as it is.
				if(strcmp(pybj->ob_type->tp_name, "metaffi_handle") == 0)
				{
					PyObject* pyhandle = PyObject_GetAttrString(pybj, "handle");
					if(!pyhandle)
					{
						throw std::runtime_error("handle attribute is not found in metaffi_handle object");
					}
					
					PyObject* runtime_id = PyObject_GetAttrString(pybj, "runtime_id");
					if(!runtime_id)
					{
						throw std::runtime_error("runtime_id attribute is not found in metaffi_handle object");
					}
					
					auto res = cdt_metaffi_handle{(metaffi_handle)PyLong_AsUnsignedLongLong(pyhandle),
					                              PyLong_AsUnsignedLongLong(runtime_id)};
					
					return res;
				}
				
				// a python object
				python3_objects_table::instance().set(pybj);
			}
			
			return {(metaffi_handle)pybj, PYTHON3_RUNTIME_ID};
		};
		
		set_numeric_array_to_cdts<cdt_metaffi_handle>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, set_object, [](PyObject* o)->int{ return 1; });
	},
	
	[](void* values_to_set, int index, metaffi_string8& val, metaffi_size& s, int starting_index) {
		set_string_to_cdts<metaffi_string8, char>((PyObject*)values_to_set, index+starting_index, val, s, [](PyObject* o, Py_ssize_t* s)->metaffi_string8{ return (metaffi_string8)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
	},
	[](void* values_to_set, int index, metaffi_string8*& arr, metaffi_size*& strings_lengths, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_string_array_to_cdts<metaffi_string8, char>((PyObject*)values_to_set, index+starting_index, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->metaffi_string8{ return (metaffi_string8)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
	},
	
	[](void* values_to_set, int index, metaffi_string16& val, metaffi_size& s, int starting_index) {
		set_string_to_cdts<metaffi_string16, wchar_t>((PyObject*)values_to_set, index+starting_index, val, s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
	},
	[](void* values_to_set, int index, metaffi_string16*& arr, metaffi_size*& strings_lengths, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_string_array_to_cdts<metaffi_string16, wchar_t>((PyObject*)values_to_set, index+starting_index, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
	},
	[](void* values_to_set, int index, metaffi_string32& val, metaffi_size& s, int starting_index) {
		set_string_to_cdts<metaffi_string32, wchar_t>((PyObject*)values_to_set, index+starting_index, val, s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
	},
	[](void* values_to_set, int index, metaffi_string32*& arr, metaffi_size*& strings_lengths, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required, int starting_index){
		set_string_array_to_cdts<metaffi_string32, wchar_t>((PyObject*)values_to_set, index+starting_index, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
	},
	[](void* values_to_set, int index, cdt_metaffi_callable& val_to_set, int starting_index){
		PyObject* callable_func = PyTuple_GetItem((PyObject*)values_to_set, index);
		val_to_set.val = PyLong_AsVoidPtr(PyObject_GetAttrString(callable_func, "pxcall_and_context"));
		PyObject* params_tuple = PyObject_GetAttrString(callable_func, "params_metaffi_types");
		PyObject* retvals_tuple = PyObject_GetAttrString(callable_func, "retval_metaffi_types");

		if(!PyTuple_Check(params_tuple)){ throw std::runtime_error("params_metaffi_types is not a tuple"); }
		if(!PyTuple_Check(retvals_tuple)){ throw std::runtime_error("retval_metaffi_types is not a tuple"); }

		val_to_set.params_types_length = PyTuple_Size(params_tuple);
		val_to_set.retval_types_length = PyTuple_Size(retvals_tuple);

		if(val_to_set.params_types_length > 0){
			val_to_set.parameters_types = new metaffi_type[val_to_set.params_types_length];
		}

		if(val_to_set.retval_types_length > 0){
			val_to_set.retval_types = new metaffi_type[val_to_set.retval_types_length];
		}

		for(Py_ssize_t i=0 ; i < val_to_set.params_types_length ; i++)
		{
			PyObject* item = PyTuple_GetItem(params_tuple, i);
			if (!PyLong_Check(item)){ throw std::runtime_error("item in params_types is not PyLong"); }
			val_to_set.parameters_types[i] = PyLong_AsUnsignedLongLong(item);
		}

		for(Py_ssize_t i=0 ; i < val_to_set.retval_types_length ; i++)
		{
			PyObject* item = PyTuple_GetItem(retvals_tuple, i);
			if (!PyLong_Check(item)){ throw std::runtime_error("item in retval_types is not PyLong"); }
			val_to_set.retval_types[i] = PyLong_AsUnsignedLongLong(item);
		}
	},
	[](int index,void* values_to_set)->metaffi_type{ // dynamic resolution of type
		return cdts_python3::get_metaffi_type((PyObject*)values_to_set);
	}
};


cdts_parse_callbacks cdts_python3::parse_callback
{
	[](void* values_to_set, int index, const metaffi_float32& val) { set_numeric_to_tuple<metaffi_float32>((PyObject*) values_to_set, index, val, PyFloat_FromDouble); },
	[](void* values_to_set, int index, const metaffi_float32* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_float32> arr_wrap((metaffi_float32*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_float32>((PyObject*) values_to_set, index, arr_wrap, PyFloat_FromDouble);
	},
	
	[](void* values_to_set, int index, const metaffi_float64& val) { set_numeric_to_tuple<metaffi_float64>((PyObject*) values_to_set, index, val, PyFloat_FromDouble); },
	[](void* values_to_set, int index, const metaffi_float64* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_float64> arr_wrap((metaffi_float64*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_float64>((PyObject*) values_to_set, index, arr_wrap, PyFloat_FromDouble);
	},
	
	[](void* values_to_set, int index, const metaffi_int8& val) { set_numeric_to_tuple<metaffi_int8>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
	[](void* values_to_set, int index, const metaffi_int8* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_int8> arr_wrap((metaffi_int8*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_int8>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
	},
	
	[](void* values_to_set, int index, const metaffi_int16& val) { set_numeric_to_tuple<metaffi_int16>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
	[](void* values_to_set, int index, const metaffi_int16* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_int16> arr_wrap((metaffi_int16*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_int16>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
	},
	
	[](void* values_to_set, int index, const metaffi_int32& val) { set_numeric_to_tuple<metaffi_int32>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
	[](void* values_to_set, int index, const metaffi_int32* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_int32> arr_wrap((metaffi_int32*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_int32>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
	},
	
	[](void* values_to_set, int index, const metaffi_int64& val) { set_numeric_to_tuple<metaffi_int64>((PyObject*) values_to_set, index, val, PyLong_FromLongLong); },
	[](void* values_to_set, int index, const metaffi_int64* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_int64> arr_wrap((metaffi_int64*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_int64>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLongLong);
	},
	
	[](void* values_to_set, int index, const metaffi_uint8& val) { set_numeric_to_tuple<metaffi_uint8>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
	[](void* values_to_set, int index, const metaffi_uint8* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_uint8> arr_wrap((metaffi_uint8*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_uint8>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
	},
	
	[](void* values_to_set, int index, const metaffi_uint16& val) { set_numeric_to_tuple<metaffi_uint16>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
	[](void* values_to_set, int index, const metaffi_uint16* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_uint16> arr_wrap((metaffi_uint16*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_uint16>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
	},
	
	[](void* values_to_set, int index, const metaffi_uint32& val) { set_numeric_to_tuple<metaffi_uint32>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
	[](void* values_to_set, int index, const metaffi_uint32* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_uint32> arr_wrap((metaffi_uint32*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_uint32>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
	},
	
	[](void* values_to_set, int index, const metaffi_uint64& val) { set_numeric_to_tuple<metaffi_uint64>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLongLong); },
	[](void* values_to_set, int index, const metaffi_uint64* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_uint64> arr_wrap((metaffi_uint64*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_uint64>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLongLong);
	},
	
	[](void* values_to_set, int index, const metaffi_bool& val)
	{
		set_numeric_to_tuple<metaffi_bool>((PyObject*)values_to_set, index, val, PyBool_FromLong);
	},
	[](void* values_to_set, int index, const metaffi_bool* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		numeric_n_array_wrapper<metaffi_bool> arr_wrap((metaffi_bool*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<metaffi_bool>((PyObject*) values_to_set, index, arr_wrap, PyBool_FromLong);
	},
	
	[](void* values_to_set, int index, const cdt_metaffi_handle& val)
	{
		auto get_object = [](const cdt_metaffi_handle& h)->PyObject*
		{
			if(h.val == nullptr){ return Py_None; }
			if(h.runtime_id != PYTHON3_RUNTIME_ID)
			{
				return new_py_metaffi_handle(h.val, h.runtime_id);
			}
			Py_IncRef((PyObject*)h.val);
			return (PyObject*)h.val;
		};
		
		set_numeric_to_tuple<cdt_metaffi_handle>((PyObject*) values_to_set, index, val, get_object);
		
	},
	[](void* values_to_set, int index, const cdt_metaffi_handle* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		auto get_object = [](const cdt_metaffi_handle& h)->PyObject*
		{
			if(h.runtime_id != PYTHON3_RUNTIME_ID)
			{
				return new_py_metaffi_handle(h.val, h.runtime_id);
			}
			Py_IncRef((PyObject*)h.val);
			return (PyObject*)h.val;
		};
		
		numeric_n_array_wrapper<cdt_metaffi_handle> arr_wrap((cdt_metaffi_handle*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_numeric_array_to_tuple<cdt_metaffi_handle>((PyObject*) values_to_set, index, arr_wrap, get_object);
	},
	
	[](void* values_to_set, int index, const metaffi_string8& val, const metaffi_size& s){
		set_string_to_tuple<metaffi_string8, char>((PyObject*) values_to_set, index, val, s, PyUnicode_FromStringAndSize);
	},
	[](void* values_to_set, int index, const metaffi_string8* arr, const metaffi_size* strings_lengths, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		string_n_array_wrapper<metaffi_string8> arr_wrap((metaffi_string8*)arr, (metaffi_size*)strings_lengths, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_string_array_to_tuple<metaffi_string8, char>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromStringAndSize);
	},
	
	[](void* values_to_set, int index, const metaffi_string16& val, const metaffi_size& s){
		set_string_to_tuple<metaffi_string16, wchar_t>((PyObject*)values_to_set, index, val, s, PyUnicode_FromWideChar);
	},
	[](void* values_to_set, int index, const metaffi_string16* arr, const metaffi_size* strings_lengths, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		string_n_array_wrapper<metaffi_string16> arr_wrap((metaffi_string16*)arr, (metaffi_size*)strings_lengths, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_string_array_to_tuple<metaffi_string16, wchar_t>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromWideChar);
	},
	
	[](void* values_to_set, int index, const metaffi_string32& val, const metaffi_size& s){
		set_string_to_tuple<metaffi_string32, wchar_t>((PyObject*)values_to_set, index, val, s, PyUnicode_FromWideChar);
	},
	[](void* values_to_set, int index, const metaffi_string32* arr, const metaffi_size* strings_lengths, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
	{
		string_n_array_wrapper<metaffi_string32> arr_wrap((metaffi_string32*)arr, (metaffi_size*)strings_lengths, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
		set_string_array_to_tuple<metaffi_string32, wchar_t>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromWideChar);
	},
	[](void* values_to_set, int index, const cdt_metaffi_callable& val_to_set)
	{
		PyRun_SimpleString(create_lambda_python_code); // make sure "create_lambda" exists
		PyObject* main_module = PyImport_AddModule("__main__");  // Get the main module
		PyObject* global_dict = PyModule_GetDict(main_module);  // Get the global dictionary

		// Get the create_lambda function
		PyObject* pyFunc = PyDict_GetItemString(global_dict, "create_lambda");

		if(!pyFunc || !PyCallable_Check(pyFunc))
		{
			throw std::runtime_error("failed to create or import create_lambda python function");
		}

		// Convert the void* to PyObject*
		PyObject* py_pxcall = PyLong_FromVoidPtr(((void**)val_to_set.val)[0]);
		PyObject* py_pcontext = PyLong_FromVoidPtr(((void**)val_to_set.val)[1]);

		// Convert the metaffi_type arrays to PyTuple*
		PyObject* py_param_types = PyTuple_New(val_to_set.params_types_length);
		for (int i = 0; i < val_to_set.params_types_length; i++) {
			PyTuple_SetItem(py_param_types, i, PyLong_FromUnsignedLongLong(val_to_set.parameters_types[i]));
		}

		PyObject* py_retval_types = PyTuple_New(val_to_set.retval_types_length);
		for (int i = 0; i < val_to_set.retval_types_length; i++) {
			PyTuple_SetItem(py_retval_types, i, PyLong_FromUnsignedLongLong(val_to_set.retval_types[i]));
		}

		PyObject* argsTuple = PyTuple_New(4);  // Create a tuple to hold the arguments

		PyTuple_SetItem(argsTuple, 0, py_pxcall);
		PyTuple_SetItem(argsTuple, 1, py_pcontext);
		PyTuple_SetItem(argsTuple, 2, py_param_types);
		PyTuple_SetItem(argsTuple, 3, py_retval_types);

		// Call the Python function
		PyObject* result = PyObject_CallObject(pyFunc, argsTuple);
		Py_DECREF(argsTuple);

		std::string err = check_python_error();
		if(!err.empty())
		{
			throw std::runtime_error(err);
		}

		PyTuple_SetItem((PyObject*)values_to_set, index, result); // set lambda in return values tuple
	}
};


std::unordered_map<std::string, metaffi_types> cdts_python3::pytypes_to_metaffi_types =
{
	{"str", metaffi_string8_type},
	{"int", metaffi_int64_type},
	{"float", metaffi_float64_type},
	{"bool", metaffi_bool_type},
	{"list", metaffi_any_type},
	{"tuple", metaffi_any_type},
};

//--------------------------------------------------------------------
cdts_python3::cdts_python3(cdt* cdts, metaffi_size cdts_length): cdts(cdts, cdts_length)
{
}
//--------------------------------------------------------------------
cdt* cdts_python3::get_cdts()
{
	return this->cdts.get_cdts();
}
//--------------------------------------------------------------------
PyObject* cdts_python3::parse()
{
	PyObject* res = PyTuple_New((Py_ssize_t)this->cdts.get_cdts_length());
	this->cdts.parse(res, parse_callback);
	return res;
}
//--------------------------------------------------------------------
thread_local metaffi_type_with_alias g_param_types[50];
void cdts_python3::build(PyObject* tuple, PyObject* tuple_types, int starting_index)
{

#ifdef _DEBUG
	if (!PyTuple_Check(tuple))
	{
		throw std::runtime_error("data is not in a tuple type");
	}

	if ((PyTuple_Size(tuple)-starting_index) != PyTuple_Size(tuple_types))
	{
		throw std::runtime_error("metaffi_types list has a different size of parameters list");
	}
#endif

	// convert tuple types to metaffi_types[]
	Py_ssize_t types_length = PyTuple_Size(tuple_types);
	metaffi_types_with_alias_ptr types = types_length > 50 ? new metaffi_type_with_alias[types_length] : g_param_types;
	for(Py_ssize_t i = 0 ; i < types_length ; i++)
	{
		PyObject* pytype = PyTuple_GetItem(tuple_types, i);
		types[i].type = (metaffi_types)PyLong_AsLong(pytype);
		types[i].alias = nullptr;
		types[i].alias_length = 0;

		if(types[i].type == metaffi_any_type) // if any, replace with actual type
		{
			PyObject* obj = PyTuple_GetItem(tuple, i+starting_index);
			types[i].type = get_metaffi_type(obj);
		}
	}
	
	this->cdts.build(types, types_length, tuple, starting_index, build_callback);
	
	if(types_length > 50){
		delete types;
	}
}
//--------------------------------------------------------------------
void cdts_python3::build(PyObject* tuple, metaffi_types_with_alias_ptr types, uint8_t types_length, int starting_index)
{
	this->cdts.build(types, types_length, tuple, starting_index, build_callback);
}
//--------------------------------------------------------------------
metaffi_types cdts_python3::get_metaffi_type(PyObject* obj)
{
	auto mtype = pytypes_to_metaffi_types.find(obj->ob_type->tp_name);
	if(mtype == pytypes_to_metaffi_types.end())
	{
		return metaffi_handle_type;
	}
	else if(mtype->second == metaffi_array_type)
	{
		// find "array of which type"?
		if(PyList_Check(obj))
		{
			PyObject* elem = PyList_GetItem(obj, 0);
			metaffi_type elemtype = get_metaffi_type(elem);
			return (metaffi_types)(metaffi_array_type | elemtype);
		}
		else if(PyTuple_Check(obj))
		{
			PyObject* elem = PyTuple_GetItem(obj, 0);
			metaffi_type elemtype = get_metaffi_type(elem);
			return (metaffi_types)(metaffi_array_type | elemtype);
		}
		else
		{
			return metaffi_array_type;
		}
	}
	
	return mtype->second;
}
//--------------------------------------------------------------------