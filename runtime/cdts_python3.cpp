#include "cdts_python3.h"
#include "objects_table.h"
#include "py_metaffi_handle.h"

using namespace metaffi::runtime;

//--------------------------------------------------------------------
cdts_python3::cdts_python3(cdt* cdts, metaffi_size cdts_length)
{
	this->cdts = std::make_unique<cdts_wrapper>(cdts, cdts_length);
}
//--------------------------------------------------------------------
cdts_python3::cdts_python3(metaffi_size cdt_count)
{
	this->cdts = std::make_unique<cdts_wrapper>(cdt_count);
}
//--------------------------------------------------------------------
cdt* cdts_python3::get_cdts()
{
	return this->cdts->get_cdts();
}
//--------------------------------------------------------------------
PyObject* cdts_python3::parse()
{
	PyObject* res = PyTuple_New((Py_ssize_t)this->cdts->get_cdts_length());

	cdts_parse_callbacks cps
	(
		[&](void* values_to_set, int index, const metaffi_float32& val) { set_numeric_to_tuple<metaffi_float32>((PyObject*) values_to_set, index, val, PyFloat_FromDouble); },
		[&](void* values_to_set, int index, const metaffi_float32* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_float32> arr_wrap((metaffi_float32*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_float32>((PyObject*) values_to_set, index, arr_wrap, PyFloat_FromDouble);
		},
		
		[&](void* values_to_set, int index, const metaffi_float64& val) { set_numeric_to_tuple<metaffi_float64>((PyObject*) values_to_set, index, val, PyFloat_FromDouble); },
		[&](void* values_to_set, int index, const metaffi_float64* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_float64> arr_wrap((metaffi_float64*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_float64>((PyObject*) values_to_set, index, arr_wrap, PyFloat_FromDouble);
		},
		
		[&](void* values_to_set, int index, const metaffi_int8& val) { set_numeric_to_tuple<metaffi_int8>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const metaffi_int8* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_int8> arr_wrap((metaffi_int8*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_int8>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_int16& val) { set_numeric_to_tuple<metaffi_int16>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const metaffi_int16* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_int16> arr_wrap((metaffi_int16*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_int16>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_int32& val) { set_numeric_to_tuple<metaffi_int32>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const metaffi_int32* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_int32> arr_wrap((metaffi_int32*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_int32>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_int64& val) { set_numeric_to_tuple<metaffi_int64>((PyObject*) values_to_set, index, val, PyLong_FromLongLong); },
		[&](void* values_to_set, int index, const metaffi_int64* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_int64> arr_wrap((metaffi_int64*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_int64>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLongLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_uint8& val) { set_numeric_to_tuple<metaffi_uint8>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const metaffi_uint8* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_uint8> arr_wrap((metaffi_uint8*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_uint8>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_uint16& val) { set_numeric_to_tuple<metaffi_uint16>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const metaffi_uint16* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_uint16> arr_wrap((metaffi_uint16*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_uint16>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_uint32& val) { set_numeric_to_tuple<metaffi_uint32>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const metaffi_uint32* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_uint32> arr_wrap((metaffi_uint32*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_uint32>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_uint64& val) { set_numeric_to_tuple<metaffi_uint64>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLongLong); },
		[&](void* values_to_set, int index, const metaffi_uint64* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_uint64> arr_wrap((metaffi_uint64*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_uint64>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLongLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_bool& val) { set_numeric_to_tuple<metaffi_bool>((PyObject*) values_to_set, index, val, PyBool_FromLong); },
		[&](void* values_to_set, int index, const metaffi_bool* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			numeric_n_array_wrapper<metaffi_bool> arr_wrap((metaffi_bool*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_bool>((PyObject*) values_to_set, index, arr_wrap, PyBool_FromLong);
		},
		
		[&](void* values_to_set, int index, const metaffi_handle& val)
		{
			auto get_object = [](metaffi_handle h)->PyObject*
			{
				if(h == NULL){ throw std::runtime_error("received NULL handle"); }
				
				if(!objects_table::instance().contains((PyObject*)h))
				{
					return new_py_metaffi_handle(h);
				}
				Py_IncRef((PyObject*)h);
				return (PyObject*)h;
			};
			set_numeric_to_tuple<metaffi_handle>((PyObject*) values_to_set, index, val, get_object);
		},
		[&](void* values_to_set, int index, const metaffi_handle* arr, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			auto get_object = [](metaffi_handle h)->PyObject*
			{
				if(!objects_table::instance().contains((PyObject*)h))
				{
					return new_py_metaffi_handle(h);
				}
				Py_IncRef((PyObject*)h);
				return (PyObject*)h;
			};
			
			numeric_n_array_wrapper<metaffi_handle> arr_wrap((metaffi_handle*)arr, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_numeric_array_to_tuple<metaffi_handle>((PyObject*) values_to_set, index, arr_wrap, get_object);
		},
		
		[&](void* values_to_set, int index, const metaffi_string8& val, const metaffi_size& s){
			set_string_to_tuple<metaffi_string8, char>((PyObject*) values_to_set, index, val, s, PyUnicode_FromStringAndSize);
		},
		[&](void* values_to_set, int index, const metaffi_string8* arr, const metaffi_size* strings_lengths, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			string_n_array_wrapper<metaffi_string8> arr_wrap((metaffi_string8*)arr, (metaffi_size*)strings_lengths, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_string_array_to_tuple<metaffi_string8, char>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromStringAndSize);
		},
		
		[&](void* values_to_set, int index, const metaffi_string16& val, const metaffi_size& s){
			set_string_to_tuple<metaffi_string16, wchar_t>((PyObject*)values_to_set, index, val, s, PyUnicode_FromWideChar);
		},
		[&](void* values_to_set, int index, const metaffi_string16* arr, const metaffi_size* strings_lengths, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			string_n_array_wrapper<metaffi_string16> arr_wrap((metaffi_string16*)arr, (metaffi_size*)strings_lengths, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_string_array_to_tuple<metaffi_string16, wchar_t>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromWideChar);
		},
		
		[&](void* values_to_set, int index, const metaffi_string32& val, const metaffi_size& s){
			set_string_to_tuple<metaffi_string32, wchar_t>((PyObject*)values_to_set, index, val, s, PyUnicode_FromWideChar);
		},
		[&](void* values_to_set, int index, const metaffi_string32* arr, const metaffi_size* strings_lengths, const metaffi_size* dimensions_lengths, const metaffi_size& dimensions)
		{
			string_n_array_wrapper<metaffi_string32> arr_wrap((metaffi_string32*)arr, (metaffi_size*)strings_lengths, (metaffi_size*)dimensions_lengths, (metaffi_size&)dimensions);
			set_string_array_to_tuple<metaffi_string32, wchar_t>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromWideChar);
		}
	);
	this->cdts->parse(res, cps);
	return res;
}
//--------------------------------------------------------------------
void cdts_python3::build(PyObject* tuple, PyObject* tuple_types, int starting_index)
{
	if (!PyTuple_Check(tuple))
	{
		throw std::runtime_error("data is not in a tuple type");
	}
	
	if ((PyTuple_Size(tuple)-starting_index) != PyTuple_Size(tuple_types))
	{
		throw std::runtime_error("metaffi_types list has a different size of parameters list");
	}
	
	cdts_build_callbacks cbs
	(
		[&](void* values_to_set, int index, metaffi_float32& val) {
			set_numeric_to_cdts<metaffi_float32>((PyObject*)values_to_set, index+starting_index, val, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_float32*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_float32>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_float64& val){
			set_numeric_to_cdts<metaffi_float64>((PyObject*)values_to_set, index+starting_index, val, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_float64*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_float64>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_int8& val){
			set_numeric_to_cdts<metaffi_int8>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_int8*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_int8>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_int16& val){
			set_numeric_to_cdts<metaffi_int16>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_int16*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_int16>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_int32& val) {
			set_numeric_to_cdts<metaffi_int32>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_int32*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_int32>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_int64& val) {
			set_numeric_to_cdts<metaffi_int64>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_int64*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_int64>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_uint8& val){
			set_numeric_to_cdts<metaffi_uint8>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_uint8*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_uint8>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_uint16& val){
			set_numeric_to_cdts<metaffi_uint16>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_uint16*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_uint16>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_uint32& val) {
			set_numeric_to_cdts<metaffi_uint32>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_uint32*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_uint32>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_uint64& val) {
			set_numeric_to_cdts<metaffi_uint64>((PyObject*)values_to_set, index+starting_index, val, PyLong_AsUnsignedLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_uint64*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_uint64>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_bool& val){
			set_numeric_to_cdts<metaffi_bool>((PyObject*)values_to_set, index+starting_index, val, [](PyObject* pybool)->int{ return pybool == Py_False? 0 : 1; }, [](PyObject* o)->int{ return PyBool_Check(o); });
		},
		[&](void* values_to_set, int index, metaffi_bool*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_numeric_array_to_cdts<metaffi_bool>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, [](PyObject* pybool)->int{ return pybool == Py_False? 0 : 1; }, [](PyObject* o)->int{ return PyBool_Check(o); });
		},
		
		[&](void* values_to_set, int index, metaffi_handle& val)
		{
			auto set_object = [](PyObject* pybj)->metaffi_handle
			{
				if(!objects_table::instance().contains(pybj))
				{
					// if metaffi handle - pass as it is.
					if(strcmp(pybj->ob_type->tp_name, "metaffi_handle") == 0)
					{
						PyObject* pyhandle = PyObject_GetAttrString(pybj, "handle");
						if(!pyhandle)
						{
							throw std::runtime_error("handle attribute is not found in metaffi_handle object");
						}
						
						return (metaffi_handle)PyLong_AsUnsignedLongLong(pyhandle);
					}
					
					// a python object
					objects_table::instance().set(pybj);
				}
				
				return (metaffi_handle)pybj;
			};
			
			set_numeric_to_cdts<metaffi_handle>((PyObject*)values_to_set, index+starting_index, val, set_object, [](PyObject* o)->int{ return 1; });
		},
		[&](void* values_to_set, int index, metaffi_handle*& arr, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required)
		{
			auto set_object = [](PyObject* pybj)->metaffi_handle
			{
				if(!objects_table::instance().contains(pybj))
				{
					// if metaffi handle - pass as it is.
					if(strcmp(pybj->ob_type->tp_name, "metaffi_handle") == 0)
					{
						PyObject* pyhandle = PyObject_GetAttrString(pybj, "handle");
						if(!pyhandle)
						{
							throw std::runtime_error("handle attribute is not found in metaffi_handle object");
						}
						
						return (metaffi_handle)PyLong_AsUnsignedLongLong(pyhandle);
					}
					
					// a python object
					objects_table::instance().set(pybj);
				}
				
				return (metaffi_handle)pybj;
			};
			
			set_numeric_array_to_cdts<metaffi_handle>((PyObject*)values_to_set, index+starting_index, arr, dimensions_lengths, dimensions, set_object, [](PyObject* o)->int{ return 1; });
		},
		
		[&](void* values_to_set, int index, metaffi_string8& val, metaffi_size& s) {
			set_string_to_cdts<metaffi_string8, char>((PyObject*)values_to_set, index+starting_index, val, s, [](PyObject* o, Py_ssize_t* s)->metaffi_string8{ return (metaffi_string8)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		[&](void* values_to_set, int index, metaffi_string8*& arr, metaffi_size*& strings_lengths, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_string_array_to_cdts<metaffi_string8, char>((PyObject*)values_to_set, index+starting_index, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->metaffi_string8{ return (metaffi_string8)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		
		[&](void* values_to_set, int index, metaffi_string16& val, metaffi_size& s) {
			set_string_to_cdts<metaffi_string16, wchar_t>((PyObject*)values_to_set, index+starting_index, val, s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		[&](void* values_to_set, int index, metaffi_string16*& arr, metaffi_size*& strings_lengths, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_string_array_to_cdts<metaffi_string16, wchar_t>((PyObject*)values_to_set, index+starting_index, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		
		[&](void* values_to_set, int index, metaffi_string32& val, metaffi_size& s) {
			set_string_to_cdts<metaffi_string32, wchar_t>((PyObject*)values_to_set, index+starting_index, val, s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		[&](void* values_to_set, int index, metaffi_string32*& arr, metaffi_size*& strings_lengths, metaffi_size*& dimensions_lengths, metaffi_size& dimensions, metaffi_bool& free_required){
			set_string_array_to_cdts<metaffi_string32, wchar_t>((PyObject*)values_to_set, index+starting_index, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		}
	);
	
	// convert tuple types to metaffi_types[]
	Py_ssize_t types_length = PyTuple_Size(tuple_types);
	metaffi_types* types = new metaffi_types[types_length];
	for(Py_ssize_t i = 0 ; i < PyTuple_Size(tuple_types) ; i++)
	{
		PyObject* pytype = PyTuple_GetItem(tuple_types, i);
		types[i] = (metaffi_types)PyLong_AsLong(pytype);
		
		if(types[i] == metaffi_any_type) // if any, replace with real type
		{
			PyObject* obj = PyTuple_GetItem(tuple, i+starting_index);
			types[i] = get_metaffi_type(obj);
		}
	}
	
	this->cdts->build(types, types_length, tuple, cbs);
}
//--------------------------------------------------------------------
metaffi_types cdts_python3::get_metaffi_type(PyObject* obj)
{
	if(strcmp(obj->ob_type->tp_name, "str") == 0)
	{
		return metaffi_string8_type;
	}
	else if(strcmp(obj->ob_type->tp_name, "int") == 0)
	{
		return metaffi_int64_type;
	}
	else if(strcmp(obj->ob_type->tp_name, "float") == 0)
	{
		return metaffi_float64_type;
	}
	else if(strcmp(obj->ob_type->tp_name, "bool") == 0)
	{
		return metaffi_bool_type;
	}
	else if(strcmp(obj->ob_type->tp_name, "list") == 0)
	{
		if(PyList_Size(obj) > 0)
		{
			PyObject* elem = PyList_GetItem(obj, 0);
			metaffi_type elemtype = get_metaffi_type(elem);
			return (metaffi_types)(metaffi_array_type | elemtype);
		}
		
		return metaffi_handle_array_type;
	}
	else if(strcmp(obj->ob_type->tp_name, "tuple") == 0)
	{
		if(PyTuple_Size(obj) > 0)
		{
			PyObject* elem = PyTuple_GetItem(obj, 0);
			metaffi_type elemtype = get_metaffi_type(elem);
			return (metaffi_types)(metaffi_array_type | elemtype);
		}
		
		return metaffi_handle_array_type;
	}
	else // handle
	{
		return metaffi_handle_type;
	}
}
//--------------------------------------------------------------------