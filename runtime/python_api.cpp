#include <runtime/runtime_plugin_api.h>
#include <cstdlib>
#include <cstring>
#include <utils/function_path_parser.h>
#include <utils/foreign_function.h>
#include "utils.h"
#include <boost/filesystem.hpp>
#include <Python.h>
#include <sstream>
#include <map>
#include "cdts_python3.h"

using namespace metaffi::utils;

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

std::map<int64_t, PyObject*> loaded_functions;

//--------------------------------------------------------------------
void initialize_environment()
{
	std::string curpath(boost::filesystem::current_path().string());
	std::stringstream ss;
	ss << "import sys" << std::endl;
	ss << "sys.path.append('" << curpath << "')" << std::endl;
	ss << "class metaffi_handle:" << std::endl;
	ss << "\tdef __init__(self, h):" << std::endl;
	ss << "\t\tself.handle = h" << std::endl << std::endl;
	
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
	
	initialize_environment();
}
//--------------------------------------------------------------------
void free_runtime(char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		return;
	}
	
	pyscope();
	
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
	
	metaffi::utils::function_path_parser fp(function_path);
	
	// TODO: can pymod be released?!
	PyObject* pymod = PyImport_ImportModuleEx(fp[function_path_entry_metaffi_guest_lib].c_str(), Py_None, Py_None, Py_None);
	
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
	cdt* parameters, uint64_t parameters_size,
	cdt* return_values, uint64_t return_values_size,
	char** out_err, uint64_t* out_err_length
)
{
	try
	{
		pyscope();
		
		auto it = loaded_functions.find(function_id);
		if (it == loaded_functions.end())
		{
			handle_err((char**) out_err, out_err_length, "Requested function has not been loaded");
			return;
		}
		PyObject* pyfunc = it->second;
		
		// convert CDT to Python3
		cdts_python3 params_cdts(parameters, parameters_size);
		PyObject* params = params_cdts.parse();
		scope_guard sgParams([&]()
		{
		 Py_DecRef(params);
		});
		
		// call function
		PyObject* res = PyObject_CallObject(pyfunc, params);
		
		// convert results back to CDT
		// assume types are as expected
		
		if(!res)
		{
			std::stringstream ss;
			ss << "Return NULL from Python function. Return type should be Tuple";
			handle_err_str((char**) out_err, out_err_length, ss.str());
			return;
		}
		
		// check if tuple
		if(!PyTuple_Check(res))
		{
			std::stringstream ss;
			ss << "Return value should be a tuple. Returned value type: " << res->ob_type->tp_name;
			handle_err_str((char**) out_err, out_err_length, ss.str());
			return;
		}
		
		// check 1st return value if error
		PyObject* returned_err = PyTuple_GetItem(res, 0);
		if(returned_err != Py_None)
		{
			Py_ssize_t err_len;
			const char* err = PyUnicode_AsUTF8AndSize(returned_err, &err_len);
			throw std::runtime_error(std::string(err, err_len));
		}
		
		// check 2nd return value is a tuple (of types)
		PyObject* tuple_types = PyTuple_GetItem(res, 1);
		if(!PyTuple_Check(tuple_types))
		{
			handle_err((char**) out_err, out_err_length, "tuple_types is not a tuple");
			return;
		}
		
		// 3rd - nth - return value;
		cdts_python3 return_cdts(return_values, return_values_size);
		return_cdts.build(res, tuple_types, 2);
		
	}
	catch(std::exception& err)
	{
		*out_err_length = strlen(err.what());
		*out_err = (char*)calloc(sizeof(char), *out_err_length+1);
		strncpy(*out_err, err.what(), *out_err_length);
	}
}
//--------------------------------------------------------------------