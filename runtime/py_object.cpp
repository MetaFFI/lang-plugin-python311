#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_bool.h"
#include "py_bytes.h"
#include "py_float.h"
#include "py_list.h"
#include "py_object.h"
#include "py_str.h"
#include "py_tuple.h"
#include <unordered_map>

py_object::py_object(PyObject* obj)
{
	instance = obj;
	if(!Py_IsNone(instance))
	{
		Py_XINCREF(instance);
	}
}

py_object::py_object(py_object&& other) noexcept
{
	instance = other.instance;
	other.instance = nullptr;
}

py_object& py_object::operator=(const py_object& other)
{
	if(instance == other.instance)
	{
		return *this;
	}
	
	instance = other.instance;
	if(!Py_IsNone(instance))
	{
		Py_XINCREF(instance);
	}
	return *this;
}

py_object::operator PyObject*() const
{
	return instance;
}

std::string py_object::get_type() const
{
	return get_object_type(instance);
}

void py_object::inc_ref()
{
	if(!Py_IsNone(instance))
	{
		Py_XINCREF(instance);
	}
}

void py_object::dec_ref()
{
	if(!Py_IsNone(instance))
	{
		Py_XINCREF(instance);
	}
}

py_object::~py_object()
{
	if(instance != nullptr && !Py_IsNone(instance)){
		Py_XDECREF(instance);
	}
}

PyObject* py_object::get_attribute(const char* name) const
{
	return pPyObject_GetAttrString(instance, name);
}

void py_object::set_attribute(const char *name, PyObject *val)
{
	pPyObject_SetAttrString(instance, name, val);
}

PyObject* py_object::detach()
{
	PyObject* res = instance;
	instance = nullptr;
	return res;
}

std::string py_object::get_object_type(PyObject* obj)
{
	if(obj == nullptr)
	{
		throw std::runtime_error("Null PyObject passed to check function");
	}
	
	PyObject* res_type = pPyObject_Type(obj);
	if(res_type == nullptr)
	{
		std::string err = check_python_error();
		throw std::runtime_error("Failed to get object type: " + err);
	}

	PyObject* res_type_name = pPyObject_GetAttrString(res_type, "__name__");
	Py_XDECREF(res_type);// Decrement reference count for res_type
	if(res_type_name == nullptr)
	{
		std::string err = check_python_error();
		throw std::runtime_error("Failed to get type name: " + err);
	}

	const char* res_type_name_str = pPyUnicode_AsUTF8(res_type_name);
	if(res_type_name_str == nullptr)
	{
		Py_XDECREF(res_type_name); // Decrement reference count for res_type_name
		std::string err = check_python_error();
		throw std::runtime_error("Failed to convert type name to UTF-8: " + err);
	}
	
	std::string res(res_type_name_str);
	Py_XDECREF(res_type_name);
	
	return res;
}

metaffi_type py_object::get_metaffi_type(PyObject* obj)
{
	if(py_str::check(obj)){
		return metaffi_string8_type;
	}
	else if(py_bool::check(obj)){
		return metaffi_bool_type;
	}
	else if(py_int::check(obj)){
		return metaffi_int64_type;
	}
	else if(py_float::check(obj)){
		return metaffi_float64_type;
	}
	else if(py_list::check(obj) || py_tuple::check(obj)){
		return metaffi_array_type;
	}
	else if(py_bytes::check(obj)){
		return metaffi_uint8_array_type;
	}
	else if(Py_IsNone(obj)){
		return metaffi_null_type;
	}
	else{
		return metaffi_handle_type;
	}
}