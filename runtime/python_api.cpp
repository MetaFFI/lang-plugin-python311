#include <runtime/runtime_plugin_api.h>
#include <cstdlib>
#include <cstring>
#include <utils/function_path_parser.h>
#include <utils/foreign_function.h>
#include "utils.h"
#include <boost/filesystem.hpp>
#include <sstream>
#include <map>
#include "cdts_python3.h"
#include <regex>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
#include <chrono>
#include <ctime>
#include <utility>
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

std::vector<foreign_function_entrypoint_signature_params_ret_t> params_ret_functions;
std::vector<foreign_function_entrypoint_signature_params_no_ret_t> params_no_ret_functions;
std::vector<foreign_function_entrypoint_signature_no_params_ret_t> no_params_ret_functions;
std::vector<foreign_function_entrypoint_signature_no_params_no_ret_t> no_params_no_ret_functions;


extern "C"
{
	void set_entrypoint(const char* entrypoint_name, void* pfunction);
	void xcall_params_ret(int is_method, PyObject* pyfunc, cdts params_ret[2], char** out_err, uint64_t* out_err_len);
	void xcall_params_no_ret(int is_method, PyObject* pyfunc, cdts parameters[1], char** out_err, uint64_t* out_err_len);
	void xcall_no_params_ret(int is_method, PyObject* pyfunc, cdts return_values[1], char** out_err, uint64_t* out_err_len);
	void xcall_no_params_no_ret(int is_method, PyObject* pyfunc, char** out_err, uint64_t* out_err_len);
}

//--------------------------------------------------------------------
std::unordered_map<std::string, void*> foreign_entities;
void set_entrypoint(const char* entrypoint_name, void* pfunction)
{
	foreign_entities[entrypoint_name] = pfunction;
}
//--------------------------------------------------------------------
void initialize_environment()
{
	std::string curpath(boost::filesystem::current_path().string());
	std::stringstream ss;
	ss << "import sys" << std::endl;
	ss << "class metaffi_handle:" << std::endl;
	ss << "\tdef __init__(self, h):" << std::endl;
	ss << "\t\tself.handle = h" << std::endl << std::endl;
	PyRun_SimpleString(ss.str().c_str());
	
	PyObject *sys_path = PySys_GetObject("path");
	PyList_Append(sys_path, PyUnicode_FromString(curpath.c_str()));
	PyList_Append(sys_path, PyUnicode_FromString(getenv("METAFFI_HOME")));
	PySys_SetObject("path", sys_path);
	
}
//--------------------------------------------------------------------
PyThreadState* _save = nullptr;
bool g_loaded = false;
void load_runtime(char** err, uint32_t* err_len)
{
	if(g_loaded){
		return;
	}
	
	// load python runtime
	if(!Py_IsInitialized())
	{
		Py_InitializeEx(0); // Do not install signal handlers
		
		// https://stackoverflow.com/questions/75846775/embedded-python-3-10-py-finalizeex-hangs-deadlock-on-threading-shutdown/
		PyEval_SaveThread();
	}
	
	
	auto gil = PyGILState_Ensure();
	
	initialize_environment();
	g_loaded = true;
	
	//_save = PyEval_SaveThread();
	PyGILState_Release(gil);
	
}
//--------------------------------------------------------------------
void free_runtime(char** err, uint32_t* err_len)
{
	//printf("+++ SKIP FREE PYTHON!\n");
	return; // skip freeing python due to deadlock in PyFinalizeEx
	
//	printf("++++ going to free python runtime\n");
//	if(!Py_IsInitialized())
//	{
//		return;
//	}
	
//	printf("++++ before gil ensure\n");
//	PyGILState_STATE gstate = PyGILState_Ensure();
//	std::string kill_all_objects = "import threading\n";
//	kill_all_objects += "print('active threads: {}'.format(threading.active_count()))\n";
//	kill_all_objects += "print('current thread indent: {}'.format(threading.current_thread().ident))\n";
//	kill_all_objects += "for t in threading.enumerate():\n";
//	kill_all_objects += "\tprint('get_ident: {} ; native: {}'.format(t.ident, t.native_id))\n";
//	kill_all_objects += "\tif not threading.current_thread().ident == t.ident:\n";
//	kill_all_objects += "\t\tt.join()\n"; // to make shutdown ignore these threads
//	kill_all_objects += "print('going to call _shutdown()')\n";
//	kill_all_objects += "threading._shutdown()\n";
//	kill_all_objects += "print('after _shutdown()')\n";


//  PyRun_SimpleString(kill_all_objects.c_str());
	
//	printf("++++ before gil ensure\n");
//	PyGILState_STATE gstate = PyGILState_Ensure();
//
//	printf("++++ before FinalizeEx\n");
//	int res = Py_FinalizeEx();
//	printf("++++ after FinalizeEx\n");
//	if(res == -1)
//	{
//		printf("++++ error %s !\n", *err);
//		handle_err(err, err_len, "Python finalization has failed!");
//	}
//
//	printf("+++ DONE freeing!\n");
}
//--------------------------------------------------------------------
void* load_function(const char* module_path, uint32_t module_path_len, const char* function_path, uint32_t function_path_len, int8_t params_count, int8_t retval_count, char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		load_runtime(err, err_len);
	}

	pyscope();

	metaffi::utils::function_path_parser fp(std::string(function_path, function_path_len));
	
	PyObject* pymod = PyImport_ImportModuleEx(std::string(module_path, module_path_len).c_str(), Py_None, Py_None, Py_None);

	if(!pymod)
	{
		// error has occurred
		handle_py_err(err, err_len);
		return nullptr;
	}
	
	auto entrypoint_fptr = foreign_entities.find(fp[function_path_entry_entrypoint_function]);
	if(entrypoint_fptr == foreign_entities.end())
	{
		handle_err(err, err_len, "Did not find foreign function");
		return nullptr;
	}
	
	return (void*)entrypoint_fptr->second;
	
}
//--------------------------------------------------------------------
void free_function(void* pff, char** err, uint32_t* err_len)
{
	// TODO: if all functions in a module are freed, module should be freed as well
}
//--------------------------------------------------------------------
void extract_args(int is_method, PyObject* params, PyObject*& out_params, PyObject*& out_kwargs)
{
	// calculate size of new "out_params"
	Py_ssize_t params_count = PyTuple_Size(params);
	
	if(params_count == 0)
	{
		out_params = params;
		return;
	}
	
	Py_ssize_t new_size = params_count;
	PyObject* positionalArgsTuple = nullptr;
	PyObject* keywordArgsDict = nullptr;
	for(int i=0 ; i < params_count ; i++)
	{
		
		PyObject* curParam = PyTuple_GetItem(params, i);
		
		if(((is_method && i != 0) || !is_method) && strcmp(curParam->ob_type->tp_name, "metaffi_positional_args") == 0) // found positional args
		{
			positionalArgsTuple = PyObject_GetAttrString(curParam, "t");
		}
		else if(((is_method && i != 0) || !is_method) && strcmp(curParam->ob_type->tp_name, "metaffi_keyword_args") == 0)
		{
			keywordArgsDict = curParam;
		}
	}
	
	if(!positionalArgsTuple && !keywordArgsDict) // no variadics
	{
		out_params = params;
		return;
	}
	
	if(keywordArgsDict)
	{
		out_kwargs = keywordArgsDict;
		new_size--; // remove keywordArgs from parameters tuple
	}
	
	Py_ssize_t positionalArgsTupleSize = 0;
	if(positionalArgsTuple)
	{
		positionalArgsTupleSize = PyTuple_Size(positionalArgsTuple);
		new_size += positionalArgsTupleSize;
		new_size--; // remove the parameter holding "positionalArgsTuple"
	}
	
	out_params = PyTuple_New(new_size);
	int j=0;
	for(int i=0 ; i < params_count ; i++)
	{
		PyObject* curParam = PyTuple_GetItem(params, i);
		
		if(((is_method && i != 0) || !is_method) && (strcmp(curParam->ob_type->tp_name, "metaffi_positional_args") == 0 ||
													 strcmp(curParam->ob_type->tp_name, "metaffi_keyword_args") == 0)) // found positional args
		{
			continue;
		}
		else
		{
			Py_INCREF(curParam);
			PyTuple_SetItem(out_params, j, curParam);
			j++;
		}
	}
	
	for(int i=0 ; i < positionalArgsTupleSize ; i++)
	{
		PyObject* curParam = PyTuple_GetItem(positionalArgsTuple, i);
		Py_INCREF(curParam);
		PyTuple_SetItem(out_params, j, curParam);
		j++;
	}
}
//--------------------------------------------------------------------
void xcall_params_ret(
		int is_method,
		PyObject* pyfunc,
		cdts params_ret[2],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		pyscope();
	
		// convert CDT to Python3
		cdts_python3 params_cdts(params_ret[0].pcdt, params_ret[0].len);
		PyObject* params = params_cdts.parse();
		
		// if parameter is of type "metaffi_positional_args" - pass internal tuple as "positional args"
		// if parameter is of type "metaffi_keyword_args" - pass dict as "keyword args"
		PyObject* out_params = nullptr;
		PyObject* out_kwargs = nullptr;
		
		extract_args(is_method, params, out_params, out_kwargs);
		
		scope_guard sgParams([&]()
	    {
			if(params != out_params)
			{
				Py_DecRef(params);
				
				if(out_params){
					Py_DecRef(out_params);
				}
			}
			else
			{
				Py_DecRef(params);
			}
			
			if(out_kwargs){
				Py_DecRef(out_kwargs);
			}
	    });

		// call function
		PyObject* res = nullptr;
		if(out_params || out_kwargs)
		{
			res = PyObject_Call(pyfunc, out_params, out_kwargs);
			
		}
		else
		{
			// in case of function with default values and there are no actual from caller
			res = PyObject_CallObject(pyfunc, nullptr);
		}
		
		// convert results back to CDT
		// assume types are as expected
		
		if(!res)
		{
			std::stringstream ss;
			ss << "Return NULL from Python function. Return type should be Tuple. Error: " << get_py_error();
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
		
		// check if tuple
#ifdef _DEBUG
		if(!PyTuple_Check(res))
		{
			std::stringstream ss;
			ss << "Return value should be a tuple. Returned value type: " << res->ob_type->tp_name;
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
#endif
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
#ifdef _DEBUG
		if(!PyTuple_Check(tuple_types))
		{
			handle_err((char**) out_err, out_err_len, "tuple_types is not a tuple");
			return;
		}
#endif
		// 3rd - nth - return value;
		cdts_python3 return_cdts(params_ret[1].pcdt, params_ret[1].len);
		return_cdts.build(res, tuple_types, 2);
		
	}
	catch(std::exception& err)
	{
		*out_err_len = strlen(err.what());
		*out_err = (char*)calloc(sizeof(char), *out_err_len + 1);
		strncpy(*out_err, err.what(), *out_err_len);
	}
}
//--------------------------------------------------------------------
void xcall_no_params_ret(
		int is_method,
		PyObject* pyfunc,
		cdts return_values[1],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		pyscope();
		
		
		// call function
		
		PyObject* res = PyObject_CallObject(pyfunc, nullptr);
		
		// convert results back to CDT
		// assume types are as expected
		if(!res)
		{
			std::stringstream ss;
			ss << "Return NULL from Python function. Return type should be Tuple. Error: " << get_py_error();
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
		// check if tuple
#ifdef _DEBUG
		if(!PyTuple_Check(res))
		{
			std::stringstream ss;
			ss << "Return value should be a tuple. Returned value type: " << res->ob_type->tp_name;
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
#endif
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
#ifdef _DEBUG
		if(!PyTuple_Check(tuple_types))
		{
			handle_err((char**) out_err, out_err_len, "tuple_types is not a tuple");
			return;
		}
#endif
		// 3rd - nth - return value;
		cdts_python3 return_cdts(return_values[0].pcdt, return_values[0].len);
		return_cdts.build(res, tuple_types, 2);
		
	}
	catch(std::exception& err)
	{
		*out_err_len = strlen(err.what());
		*out_err = (char*)calloc(sizeof(char), *out_err_len + 1);
		strncpy(*out_err, err.what(), *out_err_len);
	}
}
//--------------------------------------------------------------------
void xcall_params_no_ret(
		int is_method,
		PyObject* pyfunc,
		cdts parameters[1],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		pyscope();
		
		// convert CDT to Python3
		cdts_python3 params_cdts(parameters[0].pcdt, parameters[0].len);
		PyObject* params = params_cdts.parse();
		scope_guard sgParams([&]()
		                     {
			                     Py_DecRef(params);
		                     });

		// if parameter is of type "metaffi_positional_args" - pass internal tuple as "positional args"
		// if parameter is of type "metaffi_keyword_args" - pass dict as "keyword args"
		PyObject* out_params = nullptr;
		PyObject* out_kwargs = nullptr;
		extract_args(is_method, params, out_params, out_kwargs);
		
		// call function
		PyObject* res = nullptr;
		if(out_params || out_kwargs)
		{
				res = PyObject_Call(pyfunc, out_params, out_kwargs);
		}
		else
		{
			// in case of function with default values and there are no actuals from caller
			res = PyObject_CallObject(pyfunc, NULL);
		}
		
		// convert results back to CDT
		// assume types are as expected
		
		if(!res)
		{
			std::stringstream ss;
			ss << "Return NULL from Python function. Return type should be Tuple. Error: " << get_py_error();
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
		
		// check if tuple
#ifdef _DEBUG
		if(!PyTuple_Check(res))
		{
			std::stringstream ss;
			ss << "Return value should be a tuple. Returned value type: " << res->ob_type->tp_name;
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
#endif
		// check 1st return value if error
		PyObject* returned_err = PyTuple_GetItem(res, 0);
		if(returned_err != Py_None)
		{
			Py_ssize_t err_len;
			const char* err = PyUnicode_AsUTF8AndSize(returned_err, &err_len);
			throw std::runtime_error(std::string(err, err_len));
		}
		
	}
	catch(std::exception& err)
	{
		*out_err_len = strlen(err.what());
		*out_err = (char*)calloc(sizeof(char), *out_err_len + 1);
		strncpy(*out_err, err.what(), *out_err_len);
	}
}
//--------------------------------------------------------------------
void xcall_no_params_no_ret(
		int is_method,
		PyObject* pyfunc,
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		pyscope();
		
		// call function
		PyObject* res = PyObject_CallObject(pyfunc, nullptr);
		
		// convert results back to CDT
		// assume types are as expected
		if(!res)
		{
			std::stringstream ss;
			ss << "Return NULL from Python function. Return type should be Tuple. Error: " << get_py_error();
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
		// check if tuple
#ifdef _DEBUG
		if(!PyTuple_Check(res))
		{
			std::stringstream ss;
			ss << "Return value should be a tuple. Returned value type: " << res->ob_type->tp_name;
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
#endif
		// check 1st return value if error
		PyObject* returned_err = PyTuple_GetItem(res, 0);
		if(returned_err != Py_None)
		{
			Py_ssize_t err_len;
			const char* err = PyUnicode_AsUTF8AndSize(returned_err, &err_len);
			throw std::runtime_error(std::string(err, err_len));
		}
	}
	catch(std::exception& err)
	{
		*out_err_len = strlen(err.what());
		*out_err = (char*)calloc(sizeof(char), *out_err_len + 1);
		strncpy(*out_err, err.what(), *out_err_len);
	}
}
//--------------------------------------------------------------------