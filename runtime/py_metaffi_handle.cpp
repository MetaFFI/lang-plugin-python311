#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_metaffi_handle.h"
#include "utils.h"
#include <cstdio>
#include "runtime_id.h"

PyObject* new_py_metaffi_handle(metaffi_handle h, metaffi_uint64 runtime_id)
{
	PyObject* sys_mod_dict = PyImport_GetModuleDict();
	PyObject* metaffi_handle_mod = nullptr;
	
	metaffi_handle_mod = PyMapping_GetItemString(sys_mod_dict, "metaffi.metaffi_handle");
	if(!metaffi_handle_mod)
	{
		metaffi_handle_mod = PyMapping_GetItemString(sys_mod_dict, "__main__");
	}
	
	PyObject* instance = PyObject_CallMethod(metaffi_handle_mod, "metaffi_handle", "KK", h, runtime_id);
	
	if(instance == nullptr)
	{
		throw std::runtime_error("Failed to create pythonic metaffi_handle object\n");
	}
	
	return instance;
}
