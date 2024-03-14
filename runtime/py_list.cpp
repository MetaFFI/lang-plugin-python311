#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_list.h"
#include <stdexcept>
#include "utils.h"
#include "py_metaffi_handle.h"
#include "runtime_id.h"
#include "py_bytes.h"

py_list::py_list(Py_ssize_t size /*= 0*/)
{
	instance = PyList_New(size);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_list::py_list(PyObject* obj)
{
	if (!PyList_Check(obj))
	{
		std::stringstream ss;
		ss << "Object is not a list. It is " << obj->ob_type->tp_name;
		throw std::runtime_error(ss.str());
	}
	instance = obj;
}

py_list::py_list(py_list&& other) noexcept
{
	instance = other.instance;
	other.instance = nullptr;
}

py_list& py_list::operator=(const py_list& other)
{
	if (this == &other)
	{
		return *this;
	}
	
	Py_XDECREF(instance);
	instance = other.instance;
	Py_XINCREF(instance);
	return *this;
}

PyObject* py_list::operator[](int index)
{
	PyObject* item = PyList_GetItem(instance, index);
	if (!item)
	{
		throw std::runtime_error(check_python_error());
	}
	return item;
}

Py_ssize_t py_list::length() const
{
	return PyList_Size(instance);
}

void py_list::append(PyObject* obj)
{
	int res = PyList_Append(instance, obj);
	if (res == -1)
	{
		throw std::runtime_error(check_python_error());
	}
}

void py_list::get_dimensions_and_type(int& out_dimensions, std::string& out_common_type)
{
	out_dimensions = 0;
	out_common_type = "";
	PyObject* item;
	Py_ssize_t size = PyList_Size(instance);
	
	if (size == 0)
	{
		return;
	}
	
	for (Py_ssize_t i = 0; i < size; ++i)
	{
		item = PyList_GetItem(instance, i);
		if (PyList_Check(item))
		{
			py_list sublist(item);
			int sublist_dimensions;
			std::string sublist_common_type;
			sublist.get_dimensions_and_type(sublist_dimensions, sublist_common_type);
			
			if (sublist_dimensions == -1 || (out_dimensions != 0 && sublist_dimensions != out_dimensions - 1))
			{
				out_dimensions = -1;
				out_common_type = "";
				return;
			}
			
			out_dimensions = sublist_dimensions + 1;
			
			if (out_common_type.empty())
			{
				out_common_type = sublist_common_type;
			}
			else if (!sublist_common_type.empty() && sublist_common_type != out_common_type)
			{
				out_common_type = "";
			}
		}
		else
		{
			py_object obj(item);
			std::string type = obj.get_type();
			
			if (out_dimensions != 0)
			{
				out_dimensions = -1;
				out_common_type = "";
				return;
			}
			
			if (out_common_type.empty())
			{
				out_common_type = type;
			}
			else if (type != out_common_type)
			{
				out_common_type = "";
			}
		}
	}
}

py_list& py_list::operator=(PyObject* other)
{
	if (instance == other)
	{
		return *this;
	}
	
	instance = other;
	Py_XINCREF(instance);
	return *this;
}

bool py_list::check(PyObject* obj)
{
	return PyList_Check(obj);
}

py_list::py_list(py_list& other) noexcept
{
	instance = other.instance;
	Py_XINCREF(instance);
}

