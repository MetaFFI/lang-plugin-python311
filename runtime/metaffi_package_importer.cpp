#include "metaffi_package_importer.h"
#include <string>
#include "utils.h"
#include <cstdio>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

void import_metaffi_package()
{
	if(PyErr_Occurred())
	{
		printf("Error before import metaffi script\n");
		PyErr_Print();
		PyErr_Clear();
	}

	PyRun_SimpleString("from metaffi import *"); // seems like that doesn't do anythin

	if(PyErr_Occurred())
	{
		printf("Error after import metaffi module!!!!!!!! did you pip install metaffi-api? Error:\n");
		PyErr_Print();
		PyErr_Clear();
	}

	// // Import the metaffi package
	// PyObject* metaffi_module = PyImport_ImportModule("metaffi");
	// if (!metaffi_module) 
	// {
	// 	printf("++++++++++ FAILED TO IMPORT METAFFI MODULE ++++++++++\n");

	// 	std::string errmsg = check_python_error();
	// 	printf("failed to import metaffi module. did you pip install metaffi-api? Error: %s\n", errmsg.c_str());
	// 	PyErr_Clear();

	// 	// // Use Python C API to print sys.path
	// 	// PyObject* sys_module = PyImport_ImportModule("sys");
	// 	// if (sys_module)
	// 	// {
	// 	// 	PyObject* sys_dict = PyModule_GetDict(sys_module);
	// 	// 	PyObject* path = PyDict_GetItemString(sys_dict, "path");
	// 	// 	if (path)
	// 	// 	{
	// 	// 		PyObject* str_path = PyObject_Str(path);
	// 	// 		if (str_path)
	// 	// 		{
	// 	// 			const char* path_str = PyUnicode_AsUTF8(str_path);
	// 	// 			printf("sys.path: %s\n", path_str);
	// 	// 			Py_DECREF(str_path);
	// 	// 		}
	// 	// 	}
	// 	// 	Py_DECREF(sys_module);
	// 	// }
	// 	// else
	// 	// {
	// 	// 	std::string errmsg = check_python_error();
	// 	// 	throw std::runtime_error("failed to import sys module. Error: " + errmsg);
	// 	// }

	// 	throw std::runtime_error("failed to import metaffi module. did you pip install metaffi-api? Error"+errmsg);
	// }

	// // Add the metaffi module to the sys.modules dictionary
	// PyObject* sys_modules = PyImport_GetModuleDict();
	// PyDict_SetItemString(sys_modules, "metaffi", metaffi_module);
}