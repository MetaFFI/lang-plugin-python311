#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_tuple.h"
#include "utils.h"
#include <stdexcept>
#include "py_list.h"
#include "py_bytes.h"

py_tuple::py_tuple(Py_ssize_t size) : py_object()
{
	instance = pPyTuple_New(size);
	if (!instance)
	{
		throw std::runtime_error(check_python_error().c_str());
	}
}

py_tuple::py_tuple(PyObject** objects, int object_count) : py_object()
{
	instance = pPyTuple_New(object_count);
	if (!instance)
	{
		throw std::runtime_error(check_python_error().c_str());
	}
	
	for (int i = 0; i < object_count; i++)
	{
		pPyTuple_SetItem(instance, i, objects[i]);
	}
}

py_tuple::py_tuple(py_tuple&& other) noexcept : py_object(std::move(other))
{
}

py_tuple::py_tuple(PyObject* existingTuple) : py_object(existingTuple)
{
	if(check(existingTuple) == 0)
	{
		throw std::runtime_error("Object is not a tuple");
	}
	
	instance = existingTuple;
}

py_tuple::py_tuple(const py_tuple& other) : py_object()
{
	instance = pPyTuple_GetSlice(other.instance, 0, pPyTuple_Size(other.instance));
}

py_tuple& py_tuple::operator=(const py_tuple& other)
{
	if (this != &other)
	{
		Py_XDECREF(instance);
		instance = pPyTuple_GetSlice(other.instance, 0, pPyTuple_Size(other.instance));
	}
	return *this;
}

py_tuple& py_tuple::operator=(py_tuple&& other) noexcept
{
	if (this != &other)
	{
		Py_XDECREF(instance);
		instance = other.instance;
		other.instance = nullptr;
	}
	return *this;
}

PyObject* py_tuple::operator[](int index) const
{
	if (!instance)
	{
		pPyErr_SetString(NULL, "Tuple is null");
		return nullptr;
	}
	return pPyTuple_GetItem(instance, index);
}

Py_ssize_t py_tuple::size() const
{
	return pPyTuple_Size(instance);
}

void py_tuple::set_item(Py_ssize_t index, PyObject* value)
{
	if (pPyTuple_SetItem(instance, index, value) == -1)
	{
		throw std::runtime_error(check_python_error());
	}
}

bool py_tuple::check(PyObject* obj)
{
#ifdef _WIN32
	return pPyTuple_Check(obj);
#else
	return pPyTuple_Check(obj);
#endif
}

void py_tuple::get_metadata(PyObject* obj, bool& out_is_1d_array, bool& out_is_fixed_dimension, Py_ssize_t& out_size, metaffi_type& out_common_type)
{
	out_size = pPyTuple_Size(obj);
	out_is_1d_array = true;
	out_is_fixed_dimension = true;
	out_common_type = 0;
	
#define ARRAY       1
#define NOT_ARRAY   2
	
	int8_t last_item = 0;
	for(Py_ssize_t i = 0; i < out_size; i++)
	{
		PyObject* item = pPyTuple_GetItem(obj, i);
		
		if(i == 0)
		{
			out_common_type = py_object::get_metaffi_type(item);
		}
		else if(out_common_type != 0 && out_common_type != py_object::get_metaffi_type(item))
		{
			out_common_type = 0; // no common type
		}
		
		if(py_list::check(item) || py_tuple::check(item) || py_bytes::check(item))
		{
			if(i == 0)
			{
				last_item = ARRAY;
			}
			else if(last_item == NOT_ARRAY)
			{
				out_is_fixed_dimension = false;
				break;
			}
		}
		else
		{
			if(last_item == 0)
			{
				last_item = NOT_ARRAY;
			}
			else if(last_item == ARRAY)
			{
				out_is_fixed_dimension = false;
				break;
			}
		}
	}
	
	if(out_size == 0)
	{
		out_is_fixed_dimension = true;
	}
	
	if(!out_is_fixed_dimension)
	{
		out_is_1d_array = false;
	}
	else if(out_is_fixed_dimension && (out_size == 0 || last_item == NOT_ARRAY) )
	{
		out_is_1d_array = true;
	}
}

Py_ssize_t py_tuple::get_size(PyObject* obj)
{
	return pPyTuple_Size(obj);
}
