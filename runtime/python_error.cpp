#include "python_error.h"
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif


std::string check_python_error()
{
	std::string message;

	if (!PyErr_Occurred()){
		return message;
	}

	PyObject *excType, *excValue, *excTraceback = nullptr;
	PyErr_Fetch(&excType, &excValue, &excTraceback);
	PyErr_NormalizeException(&excType, &excValue, &excTraceback);

	PyObject* str_exc_type = PyObject_Repr(excType);
	PyObject* pyStr_exc_type = PyUnicode_AsEncodedString(str_exc_type, "utf-8", "Error ~");
	message = PyBytes_AS_STRING(pyStr_exc_type);

	PyObject* str_exc_value = PyObject_Repr(excValue);
	PyObject* pyStr_exc_value = PyUnicode_AsEncodedString(str_exc_value, "utf-8", "Error ~");
	message += ": " + std::string(PyBytes_AS_STRING(pyStr_exc_value));

	if (excTraceback != nullptr)
	{
		PyObject* module_name = PyUnicode_FromString("traceback");
		PyObject* pyth_module = PyImport_Import(module_name);
		PyObject* pyth_func = PyObject_GetAttrString(pyth_module, "format_tb");
		PyObject* pyth_val = PyObject_CallFunctionObjArgs(pyth_func, excTraceback, NULL);
		PyObject* pyth_str = PyUnicode_Join(PyUnicode_FromString(""), pyth_val);
		PyObject* pyStr = PyUnicode_AsEncodedString(pyth_str, "utf-8", "Error ~");
		message += "\n";
		message += PyBytes_AS_STRING(pyStr);
	}

	Py_XDECREF(excType);
	Py_XDECREF(excValue);
	Py_XDECREF(excTraceback);

	return message;
}