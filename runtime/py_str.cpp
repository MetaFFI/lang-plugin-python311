#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_str.h"
#include <string>

py_str::py_str()
{
	instance = PyUnicode_FromString("");
	if(!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_str::py_str(PyObject* obj)
{
	if(!PyUnicode_Check(obj))
	{
		throw std::runtime_error("Object is not a unicode string");
	}
	instance = obj;
}

py_str::py_str(py_str&& other) noexcept
{
	instance = other.instance;
	other.instance = nullptr;
}

py_str& py_str::operator=(const py_str& other)
{
	if(this->instance == other.instance)
	{
		return *this;
	}
	
	Py_XDECREF(instance);
	instance = other.instance;
	Py_XINCREF(instance);
	return *this;
}

Py_ssize_t py_str::length() const
{
	return PyUnicode_GetLength(instance);
}

py_str::py_str(const char8_t* s)
{
	instance = PyUnicode_FromString(reinterpret_cast<const char*>(s));
	if(!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_str::py_str(const char32_t* s)
{
	uint64_t length = std::char_traits<char32_t>::length(s);
	
	instance = PyUnicode_FromKindAndData(PyUnicode_4BYTE_KIND, s, (Py_ssize_t)length);
	if(!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_str::py_str(const char16_t* s)
{
	uint64_t length = std::char_traits<char16_t>::length(s);
	
	instance = PyUnicode_FromKindAndData(PyUnicode_2BYTE_KIND, s, (Py_ssize_t)length);
	if(!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

std::string py_str::to_utf8() const
{
	PyObject * utf8 = PyUnicode_AsUTF8String(instance);
	if(!utf8)
	{
		throw std::runtime_error(check_python_error());
	}
	char* s;
	Py_ssize_t len;
	PyBytes_AsStringAndSize(utf8, &s, &len);
	return std::string(s, len);
}

std::basic_string<char16_t> py_str::to_utf16() const
{
	PyObject * bytes = PyUnicode_AsUTF16String(instance);
	if(!bytes)
	{
		throw std::runtime_error(check_python_error());
	}
	char* s = PyBytes_AsString(bytes);
	Py_ssize_t size = PyBytes_Size(bytes);
	std::basic_string<char16_t> result(reinterpret_cast<char16_t*>(s), size / 2);
	Py_DECREF(bytes);
	return result;
}

std::basic_string<char32_t> py_str::to_utf32() const
{
	PyObject * bytes = PyUnicode_AsUTF32String(instance);
	if(!bytes)
	{
		throw std::runtime_error(check_python_error());
	}
	char* s = PyBytes_AsString(bytes);
	Py_ssize_t size = PyBytes_Size(bytes);
	std::basic_string<char32_t> result(reinterpret_cast<char32_t*>(s), size / 4);
	Py_DECREF(bytes);
	return result;
}

py_str::operator std::string() const
{
	return std::move(to_utf8());
}

py_str::operator std::basic_string<char16_t>() const
{
	return std::move(to_utf16());
}

py_str::operator std::basic_string<char32_t>() const
{
	return std::move(to_utf32());
}

bool py_str::check(PyObject* obj)
{
	return PyUnicode_Check(obj);
}
