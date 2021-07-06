#include "cdt_python3.h"

using namespace openffi::runtime;

//--------------------------------------------------------------------
cdt_python3::cdt_python3(cdt* cdts, openffi_size cdts_length)
{
	this->cdts = std::make_unique<cdts_wrapper>(cdts, cdts_length);
}
//--------------------------------------------------------------------
cdt_python3::cdt_python3(openffi_size cdt_count)
{
	this->cdts = std::make_unique<cdts_wrapper>(cdt_count);
}
//--------------------------------------------------------------------
cdt* cdt_python3::get_cdts()
{
	return this->cdts->get_cdts();
}
//--------------------------------------------------------------------
PyObject* cdt_python3::parse()
{
	PyObject* res = PyTuple_New((Py_ssize_t)this->cdts->get_cdts_length());
	
	cdts_parse_callbacks cps
	(
		[&](void* values_to_set, int index, const openffi_float32& val) { set_numeric_to_tuple<openffi_float32>((PyObject*) values_to_set, index, val, PyFloat_FromDouble); },
		[&](void* values_to_set, int index, const openffi_float32* val){ set_numeric_to_tuple<openffi_float32>((PyObject*) values_to_set, index, *val, PyFloat_FromDouble); },
		[&](void* values_to_set, int index, const openffi_float32* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_float32> arr_wrap((openffi_float32*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_float32>((PyObject*) values_to_set, index, arr_wrap, PyFloat_FromDouble);
		},
		
		[&](void* values_to_set, int index, const openffi_float64& val) { set_numeric_to_tuple<openffi_float64>((PyObject*) values_to_set, index, val, PyFloat_FromDouble); },
		[&](void* values_to_set, int index, const openffi_float64* val){ set_numeric_to_tuple<openffi_float64>((PyObject*) values_to_set, index, *val, PyFloat_FromDouble); },
		[&](void* values_to_set, int index, const openffi_float64* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_float64> arr_wrap((openffi_float64*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_float64>((PyObject*) values_to_set, index, arr_wrap, PyFloat_FromDouble);
		},
		
		[&](void* values_to_set, int index, const openffi_int8& val) { set_numeric_to_tuple<openffi_int8>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const openffi_int8* val){ set_numeric_to_tuple<openffi_int8>((PyObject*) values_to_set, index, *val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const openffi_int8* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_int8> arr_wrap((openffi_int8*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_int8>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
		},
		
		[&](void* values_to_set, int index, const openffi_int16& val) { set_numeric_to_tuple<openffi_int16>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const openffi_int16* val){ set_numeric_to_tuple<openffi_int16>((PyObject*) values_to_set, index, *val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const openffi_int16* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_int16> arr_wrap((openffi_int16*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_int16>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
		},
		
		[&](void* values_to_set, int index, const openffi_int32& val) { set_numeric_to_tuple<openffi_int32>((PyObject*) values_to_set, index, val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const openffi_int32* val){ set_numeric_to_tuple<openffi_int32>((PyObject*) values_to_set, index, *val, PyLong_FromLong); },
		[&](void* values_to_set, int index, const openffi_int32* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_int32> arr_wrap((openffi_int32*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_int32>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLong);
		},
		
		[&](void* values_to_set, int index, const openffi_int64& val) { set_numeric_to_tuple<openffi_int64>((PyObject*) values_to_set, index, val, PyLong_FromLongLong); },
		[&](void* values_to_set, int index, const openffi_int64* val){ set_numeric_to_tuple<openffi_int64>((PyObject*) values_to_set, index, *val, PyLong_FromLongLong); },
		[&](void* values_to_set, int index, const openffi_int64* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_int64> arr_wrap((openffi_int64*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_int64>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromLongLong);
		},
		
		[&](void* values_to_set, int index, const openffi_uint8& val) { set_numeric_to_tuple<openffi_uint8>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const openffi_uint8* val){ set_numeric_to_tuple<openffi_uint8>((PyObject*) values_to_set, index, *val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const openffi_uint8* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_uint8> arr_wrap((openffi_uint8*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_uint8>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
		},
		
		[&](void* values_to_set, int index, const openffi_uint16& val) { set_numeric_to_tuple<openffi_uint16>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const openffi_uint16* val){ set_numeric_to_tuple<openffi_uint16>((PyObject*) values_to_set, index, *val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const openffi_uint16* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_uint16> arr_wrap((openffi_uint16*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_uint16>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
		},
		
		[&](void* values_to_set, int index, const openffi_uint32& val) { set_numeric_to_tuple<openffi_uint32>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const openffi_uint32* val){ set_numeric_to_tuple<openffi_uint32>((PyObject*) values_to_set, index, *val, PyLong_FromUnsignedLong); },
		[&](void* values_to_set, int index, const openffi_uint32* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_uint32> arr_wrap((openffi_uint32*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_uint32>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLong);
		},
		
		[&](void* values_to_set, int index, const openffi_uint64& val) { set_numeric_to_tuple<openffi_uint64>((PyObject*) values_to_set, index, val, PyLong_FromUnsignedLongLong); },
		[&](void* values_to_set, int index, const openffi_uint64* val){ set_numeric_to_tuple<openffi_uint64>((PyObject*) values_to_set, index, *val, PyLong_FromUnsignedLongLong); },
		[&](void* values_to_set, int index, const openffi_uint64* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_uint64> arr_wrap((openffi_uint64*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_uint64>((PyObject*) values_to_set, index, arr_wrap, PyLong_FromUnsignedLongLong);
		},
		
		[&](void* values_to_set, int index, const openffi_bool& val) { set_numeric_to_tuple<openffi_bool>((PyObject*) values_to_set, index, val, PyBool_FromLong); },
		[&](void* values_to_set, int index, const openffi_bool* val){ set_numeric_to_tuple<openffi_bool>((PyObject*) values_to_set, index, *val, PyBool_FromLong); },
		[&](void* values_to_set, int index, const openffi_bool* arr, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			numeric_n_array_wrapper<openffi_bool> arr_wrap((openffi_bool*)arr, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_numeric_array_to_tuple<openffi_bool>((PyObject*) values_to_set, index, arr_wrap, PyBool_FromLong);
		},
		
		
		[&](void* values_to_set, int index, const openffi_string& val, const openffi_size& s){
			set_string_to_tuple<openffi_string, char>((PyObject*) values_to_set, index, val, s, PyUnicode_FromStringAndSize);
		},
		[&](void* values_to_set, int index, const openffi_string* val, const openffi_size* s) {
			set_string_to_tuple<openffi_string, char>((PyObject*) values_to_set, index, *val, *s, PyUnicode_FromStringAndSize);
		},
		[&](void* values_to_set, int index, const openffi_string* arr, const openffi_size* strings_lengths, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			string_n_array_wrapper<openffi_string> arr_wrap((openffi_string*)arr, (openffi_size*)strings_lengths, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_string_array_to_tuple<openffi_string, char>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromStringAndSize);
		},
		
		[&](void* values_to_set, int index, const openffi_string8& val, const openffi_size& s){
			set_string_to_tuple<openffi_string8, char>((PyObject*) values_to_set, index, val, s, PyUnicode_FromStringAndSize);
		},
		[&](void* values_to_set, int index, const openffi_string8* val, const openffi_size* s) {
			set_string_to_tuple<openffi_string8, char>((PyObject*) values_to_set, index, *val, *s, PyUnicode_FromStringAndSize);
		},
		[&](void* values_to_set, int index, const openffi_string8* arr, const openffi_size* strings_lengths, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			string_n_array_wrapper<openffi_string8> arr_wrap((openffi_string8*)arr, (openffi_size*)strings_lengths, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_string_array_to_tuple<openffi_string8, char>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromStringAndSize);
		},
		
		
		[&](void* values_to_set, int index, const openffi_string16& val, const openffi_size& s){
			set_string_to_tuple<openffi_string16, wchar_t>((PyObject*)values_to_set, index, val, s, PyUnicode_FromWideChar);
		},
		[&](void* values_to_set, int index, const openffi_string16* val, const openffi_size* s) {
			set_string_to_tuple<openffi_string16, wchar_t>((PyObject*)values_to_set, index, *val, *s, PyUnicode_FromWideChar);
		},
		[&](void* values_to_set, int index, const openffi_string16* arr, const openffi_size* strings_lengths, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			string_n_array_wrapper<openffi_string16> arr_wrap((openffi_string16*)arr, (openffi_size*)strings_lengths, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_string_array_to_tuple<openffi_string16, wchar_t>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromWideChar);
		},
		
		[&](void* values_to_set, int index, const openffi_string32& val, const openffi_size& s){
			set_string_to_tuple<openffi_string32, wchar_t>((PyObject*)values_to_set, index, val, s, PyUnicode_FromWideChar);
		},
		[&](void* values_to_set, int index, const openffi_string32* val, const openffi_size* s) {
			set_string_to_tuple<openffi_string32, wchar_t>((PyObject*)values_to_set, index, *val, *s, PyUnicode_FromWideChar);
		},
		[&](void* values_to_set, int index, const openffi_string32* arr, const openffi_size* strings_lengths, const openffi_size* dimensions_lengths, const openffi_size& dimensions)
		{
			string_n_array_wrapper<openffi_string32> arr_wrap((openffi_string32*)arr, (openffi_size*)strings_lengths, (openffi_size*)dimensions_lengths, (openffi_size&)dimensions);
			set_string_array_to_tuple<openffi_string32, wchar_t>((PyObject*)values_to_set, index, arr_wrap, PyUnicode_FromWideChar);
		}
	);
	
	this->cdts->parse(res, cps);
	return res;
}
//--------------------------------------------------------------------
void cdt_python3::build(PyObject* tuple, PyObject* tuple_types, int starting_index)
{
	if (!PyTuple_Check(tuple))
	{
		throw std::runtime_error("data is not in a tuple type");
	}
	
	if ((PyTuple_Size(tuple)-2) != PyTuple_Size(tuple_types))
	{
		throw std::runtime_error("openffi_types list has a different size of parameters list");
	}
	
	cdts_build_callbacks cbs
	(
		[&](void* values_to_set, int index, openffi_float32& val) {
			set_numeric_to_cdts<openffi_float32>((PyObject*)values_to_set, index+2, val, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_float32*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_float32>((PyObject*)values_to_set, index+2, *val, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_float32*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_float32>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_float64& val){
			set_numeric_to_cdts<openffi_float64>((PyObject*)values_to_set, index+2, val, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_float64*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_float64>((PyObject*)values_to_set, index+2, *val, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_float64*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_float64>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyFloat_AsDouble, [](PyObject* o)->int{ return PyFloat_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_int8& val){
			set_numeric_to_cdts<openffi_int8>((PyObject*)values_to_set, index+2, val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int8*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_int8>((PyObject*)values_to_set, index+2, *val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int8*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_int8>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_int16& val){
			set_numeric_to_cdts<openffi_int16>((PyObject*)values_to_set, index+2, val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int16*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_int16>((PyObject*)values_to_set, index+2, *val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int16*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_int16>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		
		[&](void* values_to_set, int index, openffi_int32& val) {
			set_numeric_to_cdts<openffi_int32>((PyObject*)values_to_set, index+2, val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int32*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_int32>((PyObject*)values_to_set, index+2, *val, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int32*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_int32>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_int64& val) {
			set_numeric_to_cdts<openffi_int64>((PyObject*)values_to_set, index+2, val, PyLong_AsLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int64*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_int64>((PyObject*)values_to_set, index+2, *val, PyLong_AsLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_int64*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_int64>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		
		
		[&](void* values_to_set, int index, openffi_uint8& val){
			set_numeric_to_cdts<openffi_uint8>((PyObject*)values_to_set, index+2, val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint8*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_uint8>((PyObject*)values_to_set, index+2, *val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint8*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_uint8>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_uint16& val){
			set_numeric_to_cdts<openffi_uint16>((PyObject*)values_to_set, index+2, val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint16*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_uint16>((PyObject*)values_to_set, index+2, *val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint16*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_uint16>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_uint32& val) {
			set_numeric_to_cdts<openffi_uint32>((PyObject*)values_to_set, index+2, val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint32*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_uint32>((PyObject*)values_to_set, index+2, *val, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint32*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_uint32>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_uint64& val) {
			set_numeric_to_cdts<openffi_uint64>((PyObject*)values_to_set, index+2, val, PyLong_AsUnsignedLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint64*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_uint64>((PyObject*)values_to_set, index+2, *val, PyLong_AsUnsignedLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_uint64*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_uint64>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, PyLong_AsUnsignedLongLong, [](PyObject* o)->int{ return PyLong_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_bool& val){
			set_numeric_to_cdts<openffi_bool>((PyObject*)values_to_set, index+2, val, [](PyObject* pybool)->int{ return pybool == Py_False? 0 : 1; }, [](PyObject* o)->int{ return PyBool_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_bool*& val, openffi_bool& free_required){
			set_numeric_to_cdts<openffi_bool>((PyObject*)values_to_set, index+2, *val, [](PyObject* pybool)->int{ return pybool == Py_False? 0 : 1; }, [](PyObject* o)->int{ return PyBool_Check(o); });
		},
		[&](void* values_to_set, int index, openffi_bool*& arr, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_numeric_array_to_cdts<openffi_bool>((PyObject*)values_to_set, index+2, arr, dimensions_lengths, dimensions, [](PyObject* pybool)->int{ return pybool == Py_False? 0 : 1; }, [](PyObject* o)->int{ return PyBool_Check(o); });
		},
		
		[&](void* values_to_set, int index, openffi_string& val, openffi_size& s){
			set_string_to_cdts<openffi_string, char>((PyObject*)values_to_set, index+2, val, s, [](PyObject* o, Py_ssize_t* s)->openffi_string{ return (openffi_string)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		[&](void* values_to_set, int index, openffi_string*& val, openffi_size*& s, openffi_bool& free_required) {
			set_string_to_cdts<openffi_string, char>((PyObject*)values_to_set, index+2, *val, *s, [](PyObject* o, Py_ssize_t* s)->openffi_string{ return (openffi_string)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		[&](void* values_to_set, int index, openffi_string*& arr, openffi_size*& strings_lengths, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_string_array_to_cdts<openffi_string, char>((PyObject*)values_to_set, index+2, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->openffi_string{ return (openffi_string)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		
		[&](void* values_to_set, int index, openffi_string8& val, openffi_size& s) {
			set_string_to_cdts<openffi_string8, char>((PyObject*)values_to_set, index+2, val, s, [](PyObject* o, Py_ssize_t* s)->openffi_string8{ return (openffi_string8)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		[&](void* values_to_set, int index, openffi_string8*& val, openffi_size*& s, openffi_bool& free_required) {
			set_string_to_cdts<openffi_string8, char>((PyObject*)values_to_set, index+2, *val, *s, [](PyObject* o, Py_ssize_t* s)->openffi_string8{ return (openffi_string8)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		[&](void* values_to_set, int index, openffi_string8*& arr, openffi_size*& strings_lengths, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_string_array_to_cdts<openffi_string8, char>((PyObject*)values_to_set, index+2, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->openffi_string{ return (openffi_string)PyUnicode_AsUTF8AndSize(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, strncpy);
		},
		
		[&](void* values_to_set, int index, openffi_string16& val, openffi_size& s) {
			set_string_to_cdts<openffi_string16, wchar_t>((PyObject*)values_to_set, index+2, val, s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		[&](void* values_to_set, int index, openffi_string16*& val, openffi_size*& s, openffi_bool& free_required) {
			set_string_to_cdts<openffi_string16, wchar_t>((PyObject*)values_to_set, index+2, *val, *s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		[&](void* values_to_set, int index, openffi_string16*& arr, openffi_size*& strings_lengths, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_string_array_to_cdts<openffi_string16, wchar_t>((PyObject*)values_to_set, index+2, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		
		[&](void* values_to_set, int index, openffi_string32& val, openffi_size& s) {
			set_string_to_cdts<openffi_string32, wchar_t>((PyObject*)values_to_set, index+2, val, s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		[&](void* values_to_set, int index, openffi_string32*& val, openffi_size*& s, openffi_bool& free_required) {
			set_string_to_cdts<openffi_string32, wchar_t>((PyObject*)values_to_set, index+2, *val, *s, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		},
		[&](void* values_to_set, int index, openffi_string32*& arr, openffi_size*& strings_lengths, openffi_size*& dimensions_lengths, openffi_size& dimensions, openffi_bool& free_required){
			set_string_array_to_cdts<openffi_string32, wchar_t>((PyObject*)values_to_set, index+2, arr, strings_lengths, dimensions_lengths, dimensions, [](PyObject* o, Py_ssize_t* s)->wchar_t*{ return (wchar_t*)PyUnicode_AsWideCharString(o, s); }, [](PyObject* o)->int{ return PyUnicode_Check(o); }, wcsncpy);
		}
	);
	
	// convert tuple types to openffi_types[]
	Py_ssize_t types_length = PyTuple_Size(tuple_types);
	openffi_types* types = new openffi_types[types_length];
	for(Py_ssize_t i = 0 ; i < PyTuple_Size(tuple_types) ; i++)
	{
		PyObject* pytype = PyTuple_GetItem(tuple_types, i);
		types[i] = (openffi_types)PyLong_AsLong(pytype);
	}
	
	this->cdts->build(types, types_length, tuple, cbs);
}
//--------------------------------------------------------------------