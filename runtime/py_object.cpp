#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_object.h"
#include "py_str.h"
#include "py_list.h"
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

const char* py_object::get_type() const
{
	return instance->ob_type->tp_name;
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
	return PyObject_GetAttrString(instance, name);
}

void py_object::set_attribute(const char *name, PyObject *val)
{
	PyObject_SetAttrString(instance, name, val);
}

PyObject* py_object::detach()
{
	PyObject* res = instance;
	instance = nullptr;
	return res;
}

const char* py_object::get_object_type(PyObject* obj)
{
	return obj->ob_type->tp_name;
}

std::unordered_map<std::string, metaffi_types> pytypes_to_metaffi_types =
{
    {"str", metaffi_string8_type},
    {"int", metaffi_int64_type},
    {"float", metaffi_float64_type},
    {"bool", metaffi_bool_type},
    {"list", metaffi_array_type},
    {"tuple", metaffi_array_type},
    {"NoneType", metaffi_null_type},
    {"bytes", metaffi_uint8_array_type}
};

metaffi_type py_object::get_metaffi_type(PyObject* obj)
{
	const char* pytype = get_object_type(obj);
	
	auto mtype = pytypes_to_metaffi_types.find(pytype);
	if(mtype == pytypes_to_metaffi_types.end())
	{
		return metaffi_handle_type;
	}
	
	if(mtype->second == metaffi_string8_type ||
	   mtype->second == metaffi_bool_type ||
	   mtype->second == metaffi_int64_type ||
	   mtype->second == metaffi_float64_type ||
	   mtype->second == metaffi_null_type ||
	   mtype->second & metaffi_array_type)
	{
		return mtype->second;
	}
	else
	{
		throw std::runtime_error("shouldn't get here. Is there a type missing in the if statement?");
	}
}