#ifdef _WIN32
#include <corecrt.h> // <-- required as a python bug workaround (https://github.com/microsoft/onnxruntime/issues/9735)
#endif

#include "py_metaffi_callable.h"
#include "utils.h"
#include "py_int.h"
#include "py_tuple.h"


const char* create_lambda_python_code = R"(
if 'create_lambda' not in globals():
	import platform
	import os
	import ctypes

	def get_dynamic_lib_path_from_metaffi_home(fname:str):
		osname = platform.system()
		if os.getenv('METAFFI_HOME') is None:
			raise RuntimeError('No METAFFI_HOME environment variable')
		elif fname is None:
			raise RuntimeError('fname is None')

		if osname == 'Windows':
			return os.getenv('METAFFI_HOME') + '\\' + fname + '.dll'
		elif osname == 'Darwin':
			return os.getenv('METAFFI_HOME') + '/' + fname + '.dylib'
		else:
			return os.getenv('METAFFI_HOME') + '/' + fname + '.so'  # for everything that is not windows or mac, return .so

	if platform.system() == 'Windows':
		os.add_dll_directory(os.getenv('METAFFI_HOME')+'\\lib\\')

	xllr_python3 = ctypes.cdll.LoadLibrary(get_dynamic_lib_path_from_metaffi_home('xllr.python311'))
	xllr_python3.call_xcall.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.py_object, ctypes.py_object, ctypes.py_object]
	xllr_python3.call_xcall.restype = ctypes.py_object

	XCallParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallNoParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallNoParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint64))

	def create_lambda(pxcall, context, param_types_without_alias, retval_types_without_alias):
		if param_types_without_alias is None:
			param_types_without_alias = tuple()

		if retval_types_without_alias is None:
			retval_types_without_alias = tuple()

		if not isinstance(param_types_without_alias, tuple):
			param_types_without_alias = tuple(param_types_without_alias)

		if not isinstance(retval_types_without_alias, tuple):
			retval_types_without_alias = tuple(retval_types_without_alias)

		if len(param_types_without_alias) > 0 and len(retval_types_without_alias) > 0:
			pxcall = XCallParamsRetType(pxcall)
		elif len(param_types_without_alias) > 0 and len(retval_types_without_alias) == 0:
			pxcall = XCallParamsNoRetType(pxcall)
		elif len(param_types_without_alias) == 0 and len(retval_types_without_alias) > 0:
			pxcall = XCallNoParamsRetType(pxcall)
		else:
			pxcall = XCallNoParamsNoRetType(pxcall)

		return lambda *args: xllr_python3.call_xcall(pxcall, context, param_types_without_alias, retval_types_without_alias, None if not args else args)

)";

PyObject* py_metaffi_callable::create_lambda = nullptr;

py_metaffi_callable::py_metaffi_callable() : py_object()
{
	// initialize
	if(!create_lambda)
	{
		PyObject* main_module = PyImport_AddModule("__main__");  // Get the main module
		PyObject* global_dict = PyModule_GetDict(main_module);  // Get the global dictionary
		
		// Get the create_lambda function
		PyObject* pyFunc = PyDict_GetItemString(global_dict, "create_lambda");
		
		if(!pyFunc)
		{
			// creat_lambda_python_code is not defined
			// make sure it is created
			PyRun_SimpleString(create_lambda_python_code); // make sure "create_lambda" exists
			
			// Get the create_lambda function
			pyFunc = PyDict_GetItemString(global_dict, "create_lambda");
			if(!pyFunc)
			{
				throw std::runtime_error("failed to create or import create_lambda python function");
			}
		}
		
		if(!PyCallable_Check(pyFunc))
		{
			throw std::runtime_error("create_lambda is not callable - something is wrong");
		}
		
		create_lambda = pyFunc;
	}
}

py_metaffi_callable::py_metaffi_callable(const cdt_metaffi_callable& cdt_callable) : py_metaffi_callable()
{
	// Convert the void* to PyObject*
	py_int py_pxcall(((void**)cdt_callable.val)[0]);
	py_int py_pcontext(((void**)cdt_callable.val)[1]);
	
	// Convert the metaffi_type arrays to PyTuple*
	py_tuple py_param_types(cdt_callable.params_types_length);
	for (int i = 0; i < cdt_callable.params_types_length; i++) {
		py_param_types.set_item(i, py_int(cdt_callable.parameters_types[i]).detach());
	}
	
	py_tuple py_retval_types(cdt_callable.retval_types_length);
	for (int i = 0; i < cdt_callable.retval_types_length; i++) {
		py_retval_types.set_item(i, py_int(cdt_callable.retval_types[i]).detach());
	}
	
	// call create_lambda python function
	
	py_tuple argsTuple(4);  // Create a tuple that holds the arguments
	
	argsTuple.set_item(0, py_pxcall.detach());
	argsTuple.set_item(1, py_pcontext.detach());
	argsTuple.set_item(2, py_param_types.detach());
	argsTuple.set_item(3, py_retval_types.detach());
	
	// Call "create_lambda"
	PyObject* lambda_to_mffi_callable = PyObject_CallObject(create_lambda, (PyObject*)argsTuple);
	std::string err = check_python_error();
	if(!err.empty())
	{
		throw std::runtime_error(err);
	}
	
	instance = lambda_to_mffi_callable;
}

py_metaffi_callable::py_metaffi_callable(py_metaffi_callable&& other) noexcept : py_object(std::move(other))
{
}

py_metaffi_callable& py_metaffi_callable::operator=(const py_metaffi_callable& other)
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

bool py_metaffi_callable::check(PyObject* obj)
{
	// TODO: Implement this
	std::cout << "+++ type name: " << obj->ob_type->tp_name << std::endl;
	return false;
}