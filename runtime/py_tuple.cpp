#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_tuple.h"
#include "utils.h"
#include <stdexcept>

py_tuple::py_tuple(Py_ssize_t size)
{
	instance = PyTuple_New(size);
	if (!instance)
	{
		throw std::runtime_error(check_python_error().c_str());
	}
}

py_tuple::py_tuple(PyObject** objects, int object_count)
{
	instance = PyTuple_New(object_count);
	if (!instance)
	{
		throw std::runtime_error(check_python_error().c_str());
	}
	
	for (int i = 0; i < object_count; i++)
	{
		PyTuple_SetItem(instance, i, objects[i]);
	}
}

py_tuple::py_tuple(py_tuple&& other) noexcept
{
	instance = other.instance;
	other.instance = nullptr;
}

py_tuple::py_tuple(PyObject* existingTuple)
{
	if(PyTuple_Check(existingTuple) == 0)
	{
		throw std::runtime_error("Object is not a tuple");
	}
	
	instance = existingTuple;
	Py_XINCREF(instance); // Increment reference count
}

py_tuple::py_tuple(const py_tuple& other)
{
	instance = PyTuple_GetSlice(other.instance, 0, PyTuple_Size(other.instance));
}

py_tuple& py_tuple::operator=(const py_tuple& other)
{
	if (this != &other)
	{
		Py_XDECREF(instance);
		instance = PyTuple_GetSlice(other.instance, 0, PyTuple_Size(other.instance));
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
		PyErr_SetString(PyExc_RuntimeError, "Tuple is null");
		return nullptr;
	}
	return PyTuple_GetItem(instance, index);
}

Py_ssize_t py_tuple::length() const
{
	return PyTuple_Size(instance);
}

void py_tuple::set_item(Py_ssize_t index, PyObject* value)
{
	if (PyTuple_SetItem(instance, index, value) == -1)
	{
		throw std::runtime_error(check_python_error());
	}
}