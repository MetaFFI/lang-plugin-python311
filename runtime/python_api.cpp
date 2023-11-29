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
#include <filesystem>

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

void xcall_params_no_ret(void* context, cdts parameters[1], char** out_err, uint64_t* out_err_len);
void xcall_params_ret(void* context, cdts params_ret[2], char** out_err, uint64_t* out_err_len);
void xcall_no_params_ret(void* context, cdts return_values[1], char** out_err, uint64_t* out_err_len);
void xcall_no_params_no_ret(void* context, char** out_err, uint64_t* out_err_len);

enum attribute_action
{
	attribute_action_getter,
	attribute_action_setter
};

struct python3_context
{
	attribute_action foreign_object_type;
	bool is_instance_required;
	std::vector<std::string> attribute_path;
	PyObject* entrypoint;
	PyObject* attribute_holder;
	std::vector<metaffi_type> params_types;
	std::vector<metaffi_type> retvals_types;
	
	[[nodiscard]] uint8_t params_count() const{ return params_types.size(); };
	[[nodiscard]] uint8_t retvals_count() const{ return retvals_types.size(); };
	
	// fills entrypoint and attribute holders
	// this returns correctly only if the context is of an attribute
	bool find_attribute_holder_and_attribute(PyObject* root)
	{
		if(attribute_path.empty())
		{
			return entrypoint && attribute_holder;
		}
		
		PyObject* parent = root;
		PyObject* pyobj = root;
		
		for(const std::string& step: attribute_path)
		{
			PyObject* temp = PyObject_GetAttrString(pyobj, step.c_str());
			parent = pyobj;
			pyobj = temp;
			if (!pyobj)
			{
				return false;
			}
		}
		
		entrypoint = PyUnicode_FromString(attribute_path[attribute_path.size()-1].c_str());
		
		attribute_holder = parent;
		attribute_path.clear();
		
		return true;
	}
};

//--------------------------------------------------------------------
std::string check_python_error()
{
	std::string message;

	if (!PyErr_Occurred()){
		return message;
	}

	PyObject *excType, *excValue, *excTraceback = nullptr;
	PyErr_Fetch(&excType, &excValue, &excTraceback);
	PyErr_NormalizeException(&excType, &excValue, &excTraceback);

	PyObject* str_exc_type = PyObject_Repr(excType);
	PyObject* pyStr_exc_type = PyUnicode_AsEncodedString(str_exc_type, "utf-8", "Error ~");
	message = PyBytes_AS_STRING(pyStr_exc_type);

	PyObject* str_exc_value = PyObject_Repr(excValue);
	PyObject* pyStr_exc_value = PyUnicode_AsEncodedString(str_exc_value, "utf-8", "Error ~");
	message += ": " + std::string(PyBytes_AS_STRING(pyStr_exc_value));

	if (excTraceback != nullptr)
	{
		PyObject* module_name = PyUnicode_FromString("traceback");
		PyObject* pyth_module = PyImport_Import(module_name);
		PyObject* pyth_func = PyObject_GetAttrString(pyth_module, "format_tb");
		PyObject* pyth_val = PyObject_CallFunctionObjArgs(pyth_func, excTraceback, NULL);
		PyObject* pyth_str = PyUnicode_Join(PyUnicode_FromString(""), pyth_val);
		PyObject* pyStr = PyUnicode_AsEncodedString(pyth_str, "utf-8", "Error ~");
		message += "\n";
		message += PyBytes_AS_STRING(pyStr);
	}

	Py_XDECREF(excType);
	Py_XDECREF(excValue);
	Py_XDECREF(excTraceback);

	return message;
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
void** load_function(const char* module_path, uint32_t module_path_len, const char* function_path, uint32_t function_path_len, metaffi_types_ptr param_types, metaffi_types_ptr ret_types, uint8_t params_count, uint8_t retval_count, char** err, uint32_t* err_len)
{
	if(!Py_IsInitialized())
	{
		load_runtime(err, err_len);
	}
	
	pyscope();
	
	metaffi::utils::function_path_parser fp(std::string(function_path, function_path_len));
	std::string modpath(module_path, module_path_len);
	std::filesystem::path p(modpath.c_str());
	std::filesystem::path dir = p.parent_path();
	std::string dir_str = dir.string();
	
	// Escape the backslashes in the string
	std::string dir_escaped = dir_str;
	for (size_t i = 0; i < dir_escaped.length(); i++)
	{
		if (dir_escaped[i] == '\\')
		{
			dir_escaped.insert(i, "\\");
			i++;
		}
	}
	
	std::stringstream ss;
	ss << "import os" << std::endl;
	ss << "if '"<<dir_escaped<<"' not in sys.path:" << std::endl;
	ss << "\tsys.path.append('"<<dir_escaped<<"')" << std::endl << std::endl;
	PyRun_SimpleString(ss.str().c_str());
	
	
	PyObject* pymod = PyImport_ImportModuleEx(p.stem().string().c_str(), Py_None, Py_None, Py_None);

	if(!pymod)
	{
		// error has occurred
		handle_py_err(err, err_len);
		return nullptr;
	}
	
	python3_context* ctxt = new python3_context();
	if(param_types){
		ctxt->params_types.insert(ctxt->params_types.end(), param_types, param_types + params_count);
	}
	if(ret_types){
		ctxt->retvals_types.insert(ctxt->retvals_types.end(), ret_types, ret_types+retval_count);
	}
	
	std::string load_symbol;
	if(fp.contains("callable"))
	{
		load_symbol = std::move(fp["callable"]);
		ctxt->is_instance_required = fp.contains("instance_required");
		
		std::vector<std::string> path_to_object;
		boost::split(path_to_object, load_symbol, boost::is_any_of("."));
		
		PyObject* pyobj = pymod;
		
		for(const std::string& step : path_to_object)
		{
			pyobj = PyObject_GetAttrString(pyobj, step.c_str());
			if(!pyobj)
			{
				std::stringstream ss;
				ss << "failed to find step \"" << step << "\" in the path " << load_symbol;
				handle_err(err, err_len, ss.str().c_str());
				return nullptr;
			}
		}
		
		if(fp.contains("callable") && !PyCallable_Check(pyobj))
		{
			handle_err(err, err_len, "Given callable is not PyCallable");
			return nullptr;
		}
		
		ctxt->entrypoint = pyobj;
	}
	else if(fp.contains("attribute"))
	{
		load_symbol = std::move(fp["attribute"]);
		ctxt->is_instance_required = fp.contains("instance_required");

		if(fp.contains("getter")){
			ctxt->foreign_object_type = attribute_action_getter;
		}
		else if(fp.contains("setter")){
			ctxt->foreign_object_type = attribute_action_setter;
		}
		else
		{
			handle_err(err, err_len, "missing getter or setter attribute in function path");
			return nullptr;
		}
		
		std::vector<std::string> path_to_object;
		boost::split(path_to_object, load_symbol, boost::is_any_of("."));
		
		ctxt->attribute_path = path_to_object;
		
		if(!ctxt->is_instance_required && !ctxt->find_attribute_holder_and_attribute(pymod))
		{
			handle_err(err, err_len, "attribute not found");
			return nullptr;
		}
	}
	else
	{
		handle_err(err, err_len, "expecting \"callable\" or \"variable\" in function path");
		return nullptr;
	}

	void* xcall_func = params_count > 0 && retval_count > 0 ? (void*)xcall_params_ret :
						params_count == 0 && retval_count > 0 ? (void*)xcall_no_params_ret :
						params_count > 0 && retval_count == 0 ? (void*)xcall_params_no_ret :
						(void*)xcall_no_params_no_ret;

	void** result = (void**)malloc(2*sizeof(void*));
	result[0] = xcall_func;
	result[1] = ctxt;

	return result;
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
	
	if(!positionalArgsTuple && !keywordArgsDict) // no variadic
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
void pyxcall_params_ret(
		python3_context* pctxt,
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
		
		extract_args(pctxt->is_instance_required, params, out_params, out_kwargs);
		
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

		// call function or set variable
		PyObject* res = nullptr;
		if(PyCallable_Check(pctxt->entrypoint))
		{
			if(out_params || out_kwargs)
			{
				res = PyObject_Call(pctxt->entrypoint, out_params, out_kwargs);
			}
			else
			{
				// in case of function with default values and there are no actuals from caller
				res = PyObject_CallObject(pctxt->entrypoint, nullptr);
			}
		}
		else // attribute
		{
			// load attribute, if not loaded yet
			if(!pctxt->attribute_path.empty())
			{
				if(!pctxt->is_instance_required)
				{
					handle_err(out_err, out_err_len, "non-object attribute should have been found");
					return;
				}
				
				if(!pctxt->find_attribute_holder_and_attribute(PyTuple_GetItem(params, 0)))
				{
					handle_err(out_err, out_err_len, "attribute not found");
					return;
				}
			}
			
			if(pctxt->foreign_object_type == attribute_action_getter)
			{
				res = PyObject_GetAttr(pctxt->attribute_holder, pctxt->entrypoint);
			}
			else if(pctxt->foreign_object_type == attribute_action_setter)
			{
				PyObject_SetAttr(pctxt->attribute_holder, pctxt->entrypoint, PyTuple_GetItem(params, 1));
			}
			else
			{
				handle_err(out_err, out_err_len, "Unknown foreign object type");
				return;
			}
		}
		
		std::string err_msg = std::move(check_python_error());
		if(!err_msg.empty())
		{
			handle_err_str((char**) out_err, out_err_len, err_msg);
			return;
		}

		// return values;
		cdts_python3 return_cdts(params_ret[1].pcdt, params_ret[1].len);
		return_cdts.build(res, &pctxt->retvals_types[0], pctxt->retvals_types.size(), 0);
		
	}
	catch(std::exception& err)
	{
		*out_err_len = strlen(err.what());
		*out_err = (char*)calloc(sizeof(char), *out_err_len + 1);
		strncpy(*out_err, err.what(), *out_err_len);
	}
}
//--------------------------------------------------------------------
void xcall_params_ret(void* context, cdts params_ret[2], char** out_err, uint64_t* out_err_len)
{
	python3_context* pctxt = (python3_context*)context;
	pyxcall_params_ret(pctxt, params_ret, out_err, out_err_len);
}
//--------------------------------------------------------------------
void pyxcall_no_params_ret(
		python3_context* pctxt,
		cdts return_values[1],
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		pyscope();
		
		PyObject* res = nullptr;
		if(pctxt->entrypoint && PyCallable_Check(pctxt->entrypoint))
		{
			res = PyObject_CallObject(pctxt->entrypoint, nullptr);
		}
		else
		{
			// load attribute, if not loaded yet
			if(!pctxt->attribute_path.empty())
			{
				handle_err(out_err, out_err_len, "non-object attribute should have been loaded")
				return;
			}
			
			if(pctxt->foreign_object_type == attribute_action_getter)
			{
				res = PyObject_GetAttr(pctxt->attribute_holder, pctxt->entrypoint);
			}
			else
			{
				handle_err(out_err, out_err_len, "Unexpected attribute action");
				return;
			}
		}
		
		// check error
		std::string err_msg = std::move(check_python_error());
		if(!err_msg.empty())
		{
			handle_err_str((char**) out_err, out_err_len, err_msg);
			return;
		}
		
		// convert results back to CDT
		// assume types are as expected
		if(!res)
		{
			std::stringstream ss;
			ss << "Expected return type. No return type returned";
			handle_err_str((char**) out_err, out_err_len, ss.str());
			return;
		}
		
		// return value;
		cdts_python3 return_cdts(return_values[1].pcdt, return_values[1].len);
		return_cdts.build(res, &pctxt->retvals_types[0], pctxt->retvals_types.size(), 0);
		
	}
	catch(std::exception& err)
	{
		*out_err_len = strlen(err.what());
		*out_err = (char*)calloc(sizeof(char), *out_err_len + 1);
		strncpy(*out_err, err.what(), *out_err_len);
	}
}
//--------------------------------------------------------------------
void xcall_no_params_ret(void* context, cdts parameters[1], char** out_err, uint64_t* out_err_len)
{
	python3_context* pctxt = (python3_context*)context;
	pyxcall_no_params_ret(pctxt, parameters, out_err, out_err_len);
}
//--------------------------------------------------------------------
void pyxcall_params_no_ret(
		python3_context* pctxt,
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
		extract_args(pctxt->is_instance_required, params, out_params, out_kwargs);
		
		// call function
		if(PyCallable_Check(pctxt->entrypoint))
		{
			if (out_params || out_kwargs)
			{
				PyObject_Call(pctxt->entrypoint, out_params, out_kwargs);
			} else
			{
				// in case of function with default values and there are no actual from caller
				PyObject_CallObject(pctxt->entrypoint, nullptr);
			}
		}
		else
		{
			// load attribute, if not loaded yet
			if(!pctxt->attribute_path.empty())
			{
				if(!pctxt->is_instance_required)
				{
					handle_err(out_err, out_err_len, "non-object attribute should have been found");
					return;
				}
				
				if(!pctxt->find_attribute_holder_and_attribute(PyTuple_GetItem(params, 0)))
				{
					handle_err(out_err, out_err_len, "attribute not found");
					return;
				}
			}
			
			if(pctxt->foreign_object_type == attribute_action_setter)
			{
				PyObject_SetAttr(pctxt->attribute_holder, pctxt->entrypoint,
								 pctxt->is_instance_required ? PyTuple_GetItem(params, 1) :
								                                PyTuple_GetItem(params, 0));
			}
			else
			{
				handle_err(out_err, out_err_len, "Unexpected attribute action");
				return;
			}
		}
		
		std::string err_msg = std::move(check_python_error());
		if(!err_msg.empty())
		{
			handle_err_str((char**) out_err, out_err_len, err_msg);
			return;
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
void xcall_params_no_ret(void* context, cdts return_values[1], char** out_err, uint64_t* out_err_len)
{
	python3_context* pctxt = (python3_context*)context;
	pyxcall_params_no_ret(pctxt, return_values, out_err, out_err_len);
}
//--------------------------------------------------------------------
void pyxcall_no_params_no_ret(
		python3_context* pctxt,
		char** out_err, uint64_t* out_err_len
)
{
	try
	{
		pyscope();
		
		if(!PyCallable_Check(pctxt->entrypoint))
		{
			handle_err(out_err, out_err_len, "Expecting callable object");
			return;
		}
		
		// call function
		PyObject* res = PyObject_CallObject(pctxt->entrypoint, nullptr);
		std::string err_msg = std::move(check_python_error());

		if(!err_msg.empty())
		{
			handle_err_str((char**) out_err, out_err_len, err_msg);
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
void xcall_no_params_no_ret(void* context, char** out_err, uint64_t* out_err_len)
{
	python3_context* pctxt = (python3_context*)context;
	pyxcall_no_params_no_ret(pctxt, out_err, out_err_len);
}
//--------------------------------------------------------------------
