#include "utils.h"
#include <utils/scope_guard.hpp>

#include "python3_api_wrapper.h"

using namespace metaffi::utils;


std::string check_python_error()
{
	std::string message;

	if (!pPyErr_Occurred()){
		return message;
	}

	PyObject *excType, *excValue, *excTraceback = nullptr;
	pPyErr_Fetch(&excType, &excValue, &excTraceback);
	pPyErr_NormalizeException(&excType, &excValue, &excTraceback);

	PyObject* str_exc_type = pPyObject_Repr(excType);
	PyObject* pyStr_exc_type = pPyUnicode_AsEncodedString(str_exc_type, "utf-8", "strict");
	message = PyBytes_AS_STRING(pyStr_exc_type);

	PyObject* str_exc_value = pPyObject_Repr(excValue);
	PyObject* pyStr_exc_value = pPyUnicode_AsEncodedString(str_exc_value, "utf-8", "strict");
	message += ": " + std::string(PyBytes_AS_STRING(pyStr_exc_value));

	if (excTraceback != nullptr)
	{
		PyObject* module_name = pPyUnicode_FromString("traceback");
		PyObject* pyth_module = pPyImport_Import(module_name);
		PyObject* pyth_func = pPyObject_GetAttrString(pyth_module, "format_tb");
		PyObject* pyth_val = pPyObject_CallFunctionObjArgs(pyth_func, excTraceback, NULL);
		PyObject* pyth_str = pPyUnicode_Join(pPyUnicode_FromString(""), pyth_val);
		PyObject* pyStr = pPyUnicode_AsEncodedString(pyth_str, "utf-8", "strict");
		message += "\n";
		message += PyBytes_AS_STRING(pyStr);
	}

	Py_XDECREF(excType);
	Py_XDECREF(excValue);
	Py_XDECREF(excTraceback);

	return message;
}
