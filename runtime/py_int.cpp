#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "python3_api_wrapper.h"
#include "py_int.h"


py_int::py_int(int8_t val) : py_object()
{
	instance = pPyLong_FromLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(int16_t val) : py_object()
{
	instance = pPyLong_FromLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(int32_t val) : py_object()
{
	instance = pPyLong_FromLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(int64_t val) : py_object()
{
	instance = pPyLong_FromLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(uint8_t val) : py_object()
{
	instance = pPyLong_FromUnsignedLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(uint16_t val) : py_object()
{
	instance = pPyLong_FromUnsignedLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(uint32_t val) : py_object()
{
	instance = pPyLong_FromUnsignedLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(uint64_t val) : py_object()
{
	instance = pPyLong_FromUnsignedLongLong(val);
	if (!instance)
	{
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(PyObject* obj) : py_object()
{
	if (!py_int::check(obj))
	{
		throw std::runtime_error("Object is not a long integer");
	}
	instance = obj;
}

py_int::py_int(void* obj) : py_object()
{
	instance = pPyLong_FromVoidPtr(obj);
	if (!instance)
	{
		std::stringstream ss;
		throw std::runtime_error(check_python_error());
	}
}

py_int::py_int(py_int&& other) noexcept
{
	instance = other.instance;
	other.instance = nullptr;
}

py_int& py_int::operator=(const py_int& other)
{
	if(this->instance == other.instance){
		return *this;
	}
	
	Py_XDECREF(instance);
	instance = other.instance;
	Py_XINCREF(instance);
	return *this;
}

py_int::operator int64_t() const
{
	return pPyLong_AsLongLong(instance);
}

py_int::operator uint64_t() const
{
	return pPyLong_AsUnsignedLongLong(instance);
}

py_int::operator int8_t() const
{
	return static_cast<int8_t>(pPyLong_AsLongLong(instance));
}

py_int::operator int16_t() const
{
	return static_cast<int16_t>(pPyLong_AsLongLong(instance));
}

py_int::operator int32_t() const
{
	return static_cast<int32_t>(pPyLong_AsLongLong(instance));
}

py_int::operator uint8_t() const
{
	return static_cast<uint8_t>(pPyLong_AsUnsignedLongLong(instance));
}

py_int::operator uint16_t() const
{
	return static_cast<uint16_t>(pPyLong_AsUnsignedLongLong(instance));
}

py_int::operator uint32_t() const
{
	return static_cast<uint32_t>(pPyLong_AsUnsignedLongLong(instance));
}

bool py_int::check(PyObject* obj)
{
	return pPyLong_Check(obj);
}