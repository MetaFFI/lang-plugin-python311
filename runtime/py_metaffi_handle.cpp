#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_metaffi_handle.h"
#include "utils.h"
#include <cstdio>

PyObject* new_py_metaffi_handle(metaffi_handle h)
{
	PyObject* sys_mod_dict = PyImport_GetModuleDict();
	PyObject* main_mod = PyMapping_GetItemString(sys_mod_dict, "__main__");
	
	PyObject* instance = PyObject_CallMethod(main_mod, "metaffi_handle", "K", h);
	if(instance == nullptr)
	{
		printf("Failed to create pythonic metaffi_handle object\n");
		return nullptr;
	}
	
	return instance;
}
