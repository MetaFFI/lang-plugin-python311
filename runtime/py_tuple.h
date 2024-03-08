#pragma once

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
#include "py_object.h"

class py_tuple : public py_object
{
public:
	explicit py_tuple(Py_ssize_t size);
	explicit py_tuple(PyObject** objects, int object_count);
	explicit py_tuple(PyObject* existingTuple); // Constructor that receives PyObject*
	py_tuple(const py_tuple& other); // Copy constructor
	py_tuple(py_tuple&& other) noexcept; // Move constructor
	py_tuple& operator=(const py_tuple& other); // Copy assignment operator
	py_tuple& operator=(py_tuple&& other) noexcept; // Move assignment operator
	
	void set_item(Py_ssize_t index, PyObject* value); // Sets the item at the given index
	
	PyObject* operator[](int index) const;
	[[nodiscard]] Py_ssize_t length() const; // Returns the length of the tuple
	
};
