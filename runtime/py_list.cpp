#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_list.h"
#include <stdexcept>
#include "utils.h"
#include "py_metaffi_handle.h"
#include "runtime_id.h"
#include "py_bytes.h"

py_list::py_list()
{
	instance = PyList_New(0);
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
	PyObject * item = PyList_GetItem(instance, index);
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

void py_list::add_handle_array(const cdt_metaffi_handle* arr, const metaffi_size* lengths, metaffi_size dimensions)
{
	if (dimensions == 1)
	{
		for (int i = 0; i < lengths[0]; ++i)
		{
			PyList_Append(instance, py_metaffi_handle::extract_pyobject_from_handle(&arr[i]));
		}
	}
	else
	{
		for (int i = 0; i < lengths[0]; ++i)
		{
			py_list sublist;
			add_handle_array(arr + i * lengths[1], lengths + 1, dimensions - 1);
			PyList_Append(instance, (PyObject*)sublist);
		}
	}
}

void py_list::add_bytes_array(metaffi_uint8* bytes, metaffi_size* bytes_lengths, metaffi_size len)
{
	if (len == 1)
	{
		py_bytes val((const char*)bytes, (Py_ssize_t)bytes_lengths[0]);
		PyList_Append(instance, val.detach());
	}
	else
	{
		for(int j = 0; j < bytes_lengths[0]; j++)
		{
			py_list sublist;
			sublist.add_bytes_array(bytes, bytes_lengths + 1, len - 1);
			PyList_Append(instance, (PyObject*)sublist);
			bytes += bytes_lengths[j];
		}
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

void py_list::get_handle_array(cdt_metaffi_handle** out_arr, metaffi_size** out_lengths, metaffi_size dimensions, metaffi_size* current_length /*= nullptr*/, cdt_metaffi_handle* current_arr /*= nullptr*/)
{
	if (dimensions == 1)
	{
		*out_lengths = current_length ? current_length : new metaffi_size[1];
		(*out_lengths)[0] = PyList_Size(instance);
		*out_arr = current_arr ? current_arr : new cdt_metaffi_handle[(*out_lengths)[0]];
		
		for (metaffi_size i = 0; i < (*out_lengths)[0]; i++)
		{
			PyObject* item = PyList_GetItem(instance, (Py_ssize_t)i);
			
			py_object obj(item);
			obj.inc_ref();
			(*out_arr)[i] = { item, PYTHON311_RUNTIME_ID, nullptr};
		}
	}
	else
	{
		*out_lengths = current_length ? current_length : new metaffi_size[dimensions];
		(*out_lengths)[0] = PyList_Size(instance);
		*out_arr = current_arr ? current_arr : new cdt_metaffi_handle[(*out_lengths)[0]];
		
		for (metaffi_size i = 0; i < (*out_lengths)[0]; i++)
		{
			PyObject* item = PyList_GetItem(instance, (Py_ssize_t)i);
			if (!PyList_Check(item))
			{
				std::stringstream ss;
				ss << "Object is not a list. It is " << item->ob_type->tp_name;
				throw std::runtime_error(ss.str());
			}
			
			py_list sublist(item);
			sublist.get_handle_array(out_arr, out_lengths, dimensions - 1, *out_lengths + 1, *out_arr + i * (*out_lengths)[1]);
		}
	}
}


void py_list::get_bytes_array(metaffi_uint8** out_arr, metaffi_size** out_lengths, metaffi_size dimensions, metaffi_size* current_length /*= nullptr*/, metaffi_uint8* current_arr /*= nullptr*/)
{
	if (dimensions == 1)
	{
		*out_lengths = current_length ? current_length : new metaffi_size[1];
		(*out_lengths)[0] = PyBytes_Size(instance);
		*out_arr = (uint8_t*)py_bytes(instance);
	}
	else
	{
		*out_lengths = current_length ? current_length : new metaffi_size[dimensions];
		(*out_lengths)[0] = PyList_Size(instance);
		*out_arr = current_arr ? current_arr : new metaffi_uint8[(*out_lengths)[0]];
		
		for (metaffi_size i = 0; i < (*out_lengths)[0]; i++)
		{
			PyObject* item = PyList_GetItem(instance, (Py_ssize_t)i);
			if(dimensions == 2)
			{
				if(!PyBytes_Check(item))
				{
					std::stringstream ss;
					ss << "Object is not a bytes. It is " << item->ob_type->tp_name;
					throw std::runtime_error(ss.str());
				}
				
				py_bytes bytes(item);
				((uint8_t**)(*out_arr))[i] = (uint8_t*)bytes;
				(*out_lengths)[i] = bytes.size();
			}
			else
			{
				if (!PyList_Check(item))
				{
					std::stringstream ss;
					ss << "Object is not a list. It is " << item->ob_type->tp_name;
					throw std::runtime_error(ss.str());
				}
				
				py_list sublist(item);
				sublist.get_bytes_array(out_arr, out_lengths, dimensions - 1, *out_lengths + 1, *out_arr + i * (*out_lengths)[1]);
			}
		}
	}
}

