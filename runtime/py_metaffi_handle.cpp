#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_metaffi_handle.h"
#include "utils.h"
#include <cstdio>
#include "runtime_id.h"

py_object py_metaffi_handle::extract_pyobject_from_handle(const cdt_metaffi_handle& cdt_handle)
{
	if(cdt_handle.val == nullptr)
	{
		return py_object(Py_None);
	}
	
	if(cdt_handle.runtime_id == PYTHON311_RUNTIME_ID)
	{
		Py_XINCREF((PyObject*)cdt_handle.val);
		return py_object((PyObject*)cdt_handle.val);
	}
	else
	{
		PyObject* sys_mod_dict = PyImport_GetModuleDict();
		PyObject* metaffi_handle_mod = nullptr;
		
		metaffi_handle_mod = PyMapping_GetItemString(sys_mod_dict, "metaffi.metaffi_handle");
		if(!metaffi_handle_mod)
		{
			metaffi_handle_mod = PyMapping_GetItemString(sys_mod_dict, "__main__");
		}
		
		PyObject* instance = PyObject_CallMethod(metaffi_handle_mod, "metaffi_handle", "KKK", cdt_handle.val, cdt_handle.runtime_id, cdt_handle.release);
		
		if(instance == nullptr)
		{
			std::string err = check_python_error();
			throw std::runtime_error("Failed to create pythonic metaffi_handle object: "+err+"\n");
		}
		
		return py_object(instance);
	}
}

py_metaffi_handle::py_metaffi_handle(PyObject* obj) : py_object(obj)
{
	if(!check(obj))
	{
		throw std::runtime_error("Object is not a metaffi handle");
	}
	
	instance = obj;
	Py_INCREF(instance);
}

py_metaffi_handle::py_metaffi_handle(py_metaffi_handle&& other) noexcept : py_object(std::move(other))
{
}

py_metaffi_handle& py_metaffi_handle::operator=(const py_metaffi_handle& other)
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

py_metaffi_handle::operator cdt_metaffi_handle() const
{
	if(instance == Py_None)
	{
		return {nullptr, 0, nullptr};
	}
	
	if(!check(instance))
	{
		throw std::runtime_error("Object is not a metaffi handle");
	}
	
	PyObject* val = PyObject_GetAttrString(instance, "handle");
	PyObject* runtime_id = PyObject_GetAttrString(instance, "runtime_id");
	
	if(val == nullptr || val == Py_None || runtime_id == nullptr || runtime_id == Py_None)
	{
		throw std::runtime_error("Failed to get metaffi handle attributes");
	}
	
	cdt_metaffi_handle cdt_handle { (metaffi_handle)PyLong_AsUnsignedLongLong(val), PyLong_AsUnsignedLongLong(runtime_id), nullptr };
	
	Py_XDECREF(val);
	Py_XDECREF(runtime_id);
	
	return cdt_handle;
}

bool py_metaffi_handle::check(PyObject* obj)
{
	return strcmp(obj->ob_type->tp_name, "metaffi_handle") == 0;
}