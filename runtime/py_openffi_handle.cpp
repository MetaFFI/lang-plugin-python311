#include "py_openffi_handle.h"
#include "utils.h"

PyObject* new_py_openffi_handle(openffi_handle h)
{
	PyObject* sys_mod_dict = PyImport_GetModuleDict();
	PyObject* main_mod = PyMapping_GetItemString(sys_mod_dict, "__main__");
	
	PyObject* instance = _PyObject_CallMethodOneArg(main_mod, PyUnicode_FromString("openffi_handle"), PyLong_FromUnsignedLongLong((uint64_t)h));
	if(instance == NULL)
	{
		throw std::runtime_error("Failed to create pythonic openffi_handle object");
	}
	
	return instance;
}
