#pragma once

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <stdexcept>
#include <string>
#include "runtime/metaffi_primitives.h"

class py_object
{
protected:
	PyObject* instance = nullptr;

protected:
	py_object() = default;
	
public:
	explicit py_object(PyObject* obj);
	py_object(py_object&& other) noexcept ;
	virtual ~py_object();
	
	py_object& operator=(const py_object& other);
	explicit operator PyObject*() const;
	
	[[nodiscard]] const char* get_type() const;
	void inc_ref();
	void dec_ref();
	[[nodiscard]] metaffi_type_info get_type_info() const;
	
	PyObject* get_attribute(const char* name) const;
	void set_attribute(const char* name, PyObject* val);
	
	// returns the internal PyObject* without releasing it
	// and removes ownership from the py_object
	[[nodiscard]] PyObject* detach();
};
