#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_object.h"
#include "py_str.h"
#include "py_list.h"
#include <unordered_map>

py_object::py_object(PyObject* obj)
{
	Py_XINCREF(instance);
	instance = obj;
}

py_object::py_object(py_object&& other) noexcept
{
	instance = other.instance;
	other.instance = nullptr;
}

py_object& py_object::operator=(const py_object& other)
{
	instance = other.instance;
	Py_XINCREF(instance);
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
	Py_XINCREF(instance);
}

void py_object::dec_ref()
{
	Py_XDECREF(instance);
}

py_object::~py_object()
{
	if(instance != nullptr){
		Py_XDECREF(instance);
	}
}

std::unordered_map<std::string, metaffi_types> pytypes_to_metaffi_types =
{
	{"str", metaffi_string8_type},
	{"int", metaffi_int64_type},
	{"float", metaffi_float64_type},
	{"bool", metaffi_bool_type},
	{"list", metaffi_array_type},
	{"NoneType", metaffi_null_type},
	
};

metaffi_type_info py_object::get_type_info() const
{
	metaffi_type_info info;
	std::string pytype = std::move(get_type());
	
	auto mtype = pytypes_to_metaffi_types.find(pytype);
	if(mtype == pytypes_to_metaffi_types.end())
	{
		info.type = metaffi_handle_type;
		return info;
	}
	
	switch(mtype->second)
	{
		case metaffi_string8_type:
		{
			info.type = metaffi_string8_type;
			return info;
		}break;
		
		case metaffi_bool_type:
		{
			info.type = metaffi_bool_type;
			return info;
		}break;
		
		case metaffi_int64_type:
		{
			info.type = metaffi_int64_type;
			return info;
		}break;
		
		case metaffi_float64_type:
		{
			info.type = metaffi_float64_type;
			return info;
		}break;
		
		case metaffi_null_type:
		{
			info.type = metaffi_null_type;
			return info;
		}break;
		
		case metaffi_array_type:
		{
			py_list list(instance);
			
			int dimensions = 0;
			std::string common_type;
			list.get_dimensions_and_type(dimensions, common_type);
			
			// not compatible with C-array, therefore pass it as a single dimension array
			// and inner lists will be handled as a single element of type metaffi_handle
			if(dimensions == -1)
			{
				dimensions = 1;
			}
			
			if(common_type.empty()) // no common type, pass as array of metaffi_handles
			{
				info.type = metaffi_handle_array_type;
				info.dimensions = dimensions;
			}
			else
			{
				auto common_py_type = pytypes_to_metaffi_types.find(common_type);
				if(common_py_type == pytypes_to_metaffi_types.end())
				{
					info.type = metaffi_handle_array_type;
					info.dimensions = dimensions;
				}
				else
				{
					info.type = common_py_type->second | metaffi_array_type;
					info.dimensions = dimensions;
				}
			}
		}break;
		
		default:
			throw std::runtime_error("should get here. if not found in pytypes_to_metaffi_types, method should return before switch statement.");
	}
	
	return info;
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
