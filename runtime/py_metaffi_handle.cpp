#ifdef _WIN32
#include <corecrt.h>// <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "metaffi_package_importer.h"
#include "py_metaffi_handle.h"
#include "runtime_id.h"
#include "utils.h"
#include <cstdio>

py_object py_metaffi_handle::extract_pyobject_from_handle(const cdt_metaffi_handle& cdt_handle)
{
	if(cdt_handle.handle == nullptr)
	{
		return py_object(pPy_None);
	}

	if(cdt_handle.runtime_id == PYTHON311_RUNTIME_ID)
	{
		Py_XINCREF((PyObject*)cdt_handle.handle);
		return py_object((PyObject*)cdt_handle.handle);
	}
	else
	{
		PyObject* sys_mod_dict = pPyImport_GetModuleDict();
		PyObject* metaffi_handle_mod = nullptr;

		pPyRun_SimpleString("import sys\nprint(f'EXECUTABLE: {sys.executable}')\nprint(f'VERSION: {sys.version}')\nprint(f'SYS.PATH {sys.path}')\n");

		metaffi_handle_mod = pPyMapping_GetItemString(sys_mod_dict, "metaffi");
		if(!metaffi_handle_mod)
		{
			pPyErr_Clear();
			import_metaffi_package();
			std::string err_during_import = check_python_error();
			if(!err_during_import.empty())
			{
				std::cerr << "Error during import metaffi: " << std::endl
				          << err_during_import << std::endl;
				throw std::runtime_error("Failed to get metaffi module");
			}

			std::cerr << "metaffi imported - try again" << std::endl;
			metaffi_handle_mod = pPyMapping_GetItemString(sys_mod_dict, "metaffi");


			if(!metaffi_handle_mod)
			{
				std::cerr << "Failed to get metaffi module" << std::endl;
				throw std::runtime_error("Failed to get metaffi module");
			}
			else
			{
				std::cerr << "Got metaffi module" << std::endl;
			}
		}

		PyObject* instance = pPyObject_CallMethod(metaffi_handle_mod, "MetaFFIHandle", "KKK", cdt_handle.handle, cdt_handle.runtime_id, cdt_handle.release);

		if(instance == nullptr)
		{
			std::string err = check_python_error();
			throw std::runtime_error("Failed to create pythonic MetaFFIHandle object: " + err + "\n");
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

cdt_metaffi_handle* py_metaffi_handle::as_cdt_metaffi_handle() const
{
	if(instance == pPy_None)
	{
		return nullptr;
	}

	if(!check(instance))
	{
		throw std::runtime_error("Object is not a metaffi handle");
	}

	PyObject* val = pPyObject_GetAttrString(instance, "handle");
	PyObject* runtime_id = pPyObject_GetAttrString(instance, "runtime_id");
	PyObject* releaser = pPyObject_GetAttrString(instance, "releaser");

	if(val == nullptr || val == pPy_None || runtime_id == nullptr || runtime_id == pPy_None || releaser == nullptr || releaser == pPy_None)
	{
		std::stringstream ss;
		ss << "Failed to get metaffi handle attributes: handle=" << val << ", runtime_id=" << runtime_id << ", releaser=" << releaser;
		throw std::runtime_error(ss.str());
	}

	cdt_metaffi_handle* cdt_handle = new cdt_metaffi_handle{(metaffi_handle)pPyLong_AsVoidPtr(val),
	                                                        pPyLong_AsUnsignedLongLong(runtime_id),
	                                                        (releaser_fptr_t)pPyLong_AsVoidPtr(releaser)};

	Py_XDECREF(val);
	Py_XDECREF(runtime_id);
	Py_XDECREF(releaser);

	return cdt_handle;
}

bool py_metaffi_handle::check(PyObject* obj)
{
	return py_object::get_object_type(obj) == "MetaFFIHandle";
}