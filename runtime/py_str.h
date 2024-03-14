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
#include "utils.h"
#include "py_object.h"

class py_str : public py_object
{
public:
	static bool check(PyObject* obj);
	
public:
	py_str();
	explicit py_str(PyObject* obj);
	explicit py_str(const char* s);
	explicit py_str(const char32_t* s);
	explicit py_str(const char16_t* s);
	py_str(py_str&& other) noexcept ;
	py_str& operator=(const py_str& other);
	[[nodiscard]] Py_ssize_t length() const;
	[[nodiscard]] std::string to_utf8() const;
	[[nodiscard]] std::basic_string<char16_t> to_utf16() const;
	[[nodiscard]] std::basic_string<char32_t> to_utf32() const;
	
	explicit operator std::string() const;
	explicit operator std::basic_string<char16_t>() const;
	explicit operator std::basic_string<char32_t>() const;
	
};
