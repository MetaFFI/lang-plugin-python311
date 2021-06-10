#include <runtime/runtime_plugin_api.h>
#include <cstdlib>
#include <cstring>
#include <utils/scope_guard.hpp>
#include <utils/function_path_parser.h>
#include <utils/foreign_function.h>
#include "utils.h"
#include <boost/filesystem.hpp>
#include <Python.h>
#include <sstream>
#include <map>

using namespace openffi::utils;

#define handle_err(err, err_len, desc) \
	*err_len = strlen( desc ); \
	*err = (char*)malloc(*err_len + 1); \
	strcpy(*err, desc ); \
	memset((*err+*err_len), 0, 1);

#define handle_err_str(err, err_len, descstr) \
	*err_len = descstr.length(); \
	*err = (char*)malloc(*err_len + 1); \
	descstr.copy(*err, *err_len, 0); \
	memset((*err+*err_len), 0, 1);

#define handle_py_err(err, err_len) \
	std::string pyerr(get_py_error()); \
	handle_err_str(err, err_len, pyerr);

#define TRUE 1
#define FALSE 0

#define pyscope()\
	PyGILState_STATE gstate = PyGILState_Ensure(); \
	scope_guard sggstate([&]() \
	{ \
		PyGILState_Release(gstate); \
	});
	
std::map<int64_t, PyObject*> loaded_functions;

//--------------------------------------------------------------------
void load_package_path()
{
	std::string curpath(boost::filesystem::current_path().string());
	std::stringstream ss;
	ss << "import sys" << std::endl;
	//ss << "import site" << std::endl;
	//ss << "sys.path.append(site.USER_SITE)" << std::endl;
	ss << "sys.path.append('" << curpath << "')" << std::endl;
	
	PyRun_SimpleString(ss.str().c_str());
}
//--------------------------------------------------------------------
void load_runtime(char** err, uint32_t* err_len)
{
	// load python runtime
	if(!Py_IsInitialized())
	{
		Py_InitializeEx(1); // 1 means register signal handlers
	}
	
	pyscope();
	
	load_package_path();
}
//--------------------------------------------------------------------
void free_runtime(char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		return;
	}
		
	int res = Py_FinalizeEx();
	if(res == -1)
	{
		handle_err(err, err_len, "Python finalization has failed!");
		return;
	}
}
//--------------------------------------------------------------------
int64_t load_function(const char* function_path, uint32_t function_path_len, char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		load_runtime(err, err_len);
	}

	pyscope();
	
	openffi::utils::function_path_parser fp(function_path);
	
	// TODO: can pymod be released?!
	PyObject* pymod = PyImport_ImportModuleEx(fp[function_path_entry_openffi_guest_lib].c_str(), Py_None, Py_None, Py_None);
	
	if(!pymod)
	{
		// error has occurred
		handle_py_err(err, err_len);
		return -1;
	}

	// load function
	PyObject* pyfunc = PyObject_GetAttrString(pymod, fp[function_path_entry_entrypoint_function].c_str());
	
	if(!pyfunc)
	{
		handle_err((char**)err, err_len, "Cannot find function in module!");
		return -1;
	}

	if(!PyCallable_Check(pyfunc))
	{
		handle_err((char**)err, err_len, "Requested function found in module, but it is not a function!");
		return -1;
	}
	
	// place in loaded functions
	int64_t function_id = loaded_functions.empty() ? 0 : -1;
	for(auto& it : loaded_functions) // make function_id the highest number
	{
		if(function_id <= it.first)
		{
			function_id = it.first + 1;
		}
	}
	
	loaded_functions[function_id] = pyfunc;
	
	return function_id;
}
//--------------------------------------------------------------------
void free_function(int64_t function_id, char** err, uint32_t* err_len)
{
	// TODO: if all functions in a module are freed, module should be freed as well
}
//--------------------------------------------------------------------
void call(
	int64_t function_id,
	void** parameters, uint64_t parameters_size,
	void** return_values, uint64_t return_values_size,
	char** out_err, uint64_t* out_err_length
)
{
	auto it = loaded_functions.find(function_id);
	if(it == loaded_functions.end())
	{
		handle_err((char**)out_err, out_err_length, "Requested function has not been loaded");
		return;
	}
	PyObject* pyfunc = it->second;
	
	// set parameters
	PyObject* paramsArray = PyTuple_New(4);
	if(!paramsArray)
	{
		handle_err((char**)out_err, out_err_length, "Failed to create new tuple");
		return;
	}
	
	scope_guard sgParams([&]()
	{
		Py_DecRef(paramsArray);
	});
	
	PyTuple_SetItem(paramsArray, 0, PyLong_FromUnsignedLongLong(reinterpret_cast<unsigned long long int>(parameters)));
	PyTuple_SetItem(paramsArray, 1, PyLong_FromUnsignedLongLong(parameters_size));
	PyTuple_SetItem(paramsArray, 2, PyLong_FromUnsignedLongLong(reinterpret_cast<unsigned long long int>(return_values)));
	PyTuple_SetItem(paramsArray, 3, PyLong_FromUnsignedLongLong(return_values_size));
	PyObject* res = PyObject_CallObject(pyfunc, paramsArray);
	
	if(res != Py_None) // returned an error (if None, there's no error)
	{
		const char* perrmsg = PyUnicode_AsUTF8(res);
		handle_err((char**)out_err, out_err_length, perrmsg);
		return;
	}
}
//--------------------------------------------------------------------