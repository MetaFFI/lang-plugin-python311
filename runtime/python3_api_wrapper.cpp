#include "python3_api_wrapper.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string.h>  // For strstr

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>// For EnumProcessModules
#else
#include <dlfcn.h>
#include <link.h>  // For dl_iterate_phdr
#include <elf.h>   // For ELF types
#include <filesystem>
#endif

// Initialize all function pointers to nullptr
PyLong_FromUnsignedLongLong_t pPyLong_FromUnsignedLongLong = nullptr;
PyBool_FromLong_t pPyBool_FromLong = nullptr;
PyBytes_FromString_t pPyBytes_FromString = nullptr;
PyBytes_FromStringAndSize_t pPyBytes_FromStringAndSize = nullptr;
PyBytes_Size_t pPyBytes_Size = nullptr;
PyBytes_AsString_t pPyBytes_AsString = nullptr;
PyBytes_AsStringAndSize_t pPyBytes_AsStringAndSize = nullptr;
PyLong_AsVoidPtr_t pPyLong_AsVoidPtr = nullptr;
PyLong_FromVoidPtr_t pPyLong_FromVoidPtr = nullptr;
PyMapping_GetItemString_t pPyMapping_GetItemString = nullptr;
PyEval_SaveThread_t pPyEval_SaveThread = nullptr;
PyEval_RestoreThread_t pPyEval_RestoreThread = nullptr;
pPyGILState_Ensure_t pPyGILState_Ensure = nullptr;
PyGILState_Release_t pPyGILState_Release = nullptr;
PyGILState_GetThisThreadState_t pPyGILState_GetThisThreadState = nullptr;
PyGILState_Check_t pPyGILState_Check = nullptr;

Py_Initialize_t pPy_Initialize = nullptr;
Py_InitializeEx_t pPy_InitializeEx = nullptr;
Py_Finalize_t pPy_Finalize = nullptr;
Py_FinalizeEx_t pPy_FinalizeEx = nullptr;
Py_IsInitialized_t pPy_IsInitialized = nullptr;

PyUnicode_FromString_t pPyUnicode_FromString = nullptr;
PyUnicode_FromStringAndSize_t pPyUnicode_FromStringAndSize = nullptr;
PyUnicode_FromFormat_t pPyUnicode_FromFormat = nullptr;
PyUnicode_AsUTF8_t pPyUnicode_AsUTF8 = nullptr;
PyUnicode_AsUTF8AndSize_t pPyUnicode_AsUTF8AndSize = nullptr;

PyLong_FromLong_t pPyLong_FromLong = nullptr;
PyLong_FromUnsignedLong_t pPyLong_FromUnsignedLong = nullptr;
PyLong_FromDouble_t pPyLong_FromDouble = nullptr;
PyLong_FromString_t pPyLong_FromString = nullptr;
PyLong_FromLongLong_t pPyLong_FromLongLong = nullptr;
PyLong_AsLong_t pPyLong_AsLong = nullptr;
PyLong_AsLongLong_t pPyLong_AsLongLong = nullptr;
PyLong_AsUnsignedLong_t pPyLong_AsUnsignedLong = nullptr;
PyLong_AsUnsignedLongLong_t pPyLong_AsUnsignedLongLong = nullptr;

PyFloat_FromDouble_t pPyFloat_FromDouble = nullptr;
PyFloat_FromString_t pPyFloat_FromString = nullptr;
PyFloat_AsDouble_t pPyFloat_AsDouble = nullptr;

PyList_New_t pPyList_New = nullptr;
PyList_Size_t pPyList_Size = nullptr;
PyList_GetItem_t pPyList_GetItem = nullptr;
PyList_SetItem_t pPyList_SetItem = nullptr;
PyList_Insert_t pPyList_Insert = nullptr;
PyList_Append_t pPyList_Append = nullptr;
PyList_GetSlice_t pPyList_GetSlice = nullptr;
PyList_SetSlice_t pPyList_SetSlice = nullptr;

PyTuple_New_t pPyTuple_New = nullptr;
PyTuple_Size_t pPyTuple_Size = nullptr;
PyTuple_GetItem_t pPyTuple_GetItem = nullptr;
PyTuple_SetItem_t pPyTuple_SetItem = nullptr;
PyTuple_GetSlice_t pPyTuple_GetSlice = nullptr;

PyDict_New_t pPyDict_New = nullptr;
PyDict_SetItem_t pPyDict_SetItem = nullptr;
PyDict_SetItemString_t pPyDict_SetItemString = nullptr;
PyDict_GetItem_t pPyDict_GetItem = nullptr;
PyDict_GetItemString_t pPyDict_GetItemString = nullptr;
PyDict_DelItem_t pPyDict_DelItem = nullptr;
PyDict_DelItemString_t pPyDict_DelItemString = nullptr;
PyDict_Clear_t pPyDict_Clear = nullptr;
PyDict_Next_t pPyDict_Next = nullptr;
PyDict_Size_t pPyDict_Size = nullptr;

PyErr_SetString_t pPyErr_SetString = nullptr;
PyErr_SetObject_t pPyErr_SetObject = nullptr;
PyErr_Occurred_t pPyErr_Occurred = nullptr;
PyErr_Clear_t pPyErr_Clear = nullptr;
PyErr_Print_t pPyErr_Print = nullptr;
PyErr_WriteUnraisable_t pPyErr_WriteUnraisable = nullptr;
PyErr_ExceptionMatches_t pPyErr_ExceptionMatches = nullptr;
PyErr_GivenExceptionMatches_t pPyErr_GivenExceptionMatches = nullptr;
PyErr_Fetch_t pPyErr_Fetch = nullptr;
PyErr_Restore_t pPyErr_Restore = nullptr;

PyObject_Print_t pPyObject_Print = nullptr;
PyObject_HasAttrString_t pPyObject_HasAttrString = nullptr;
PyObject_GetAttrString_t pPyObject_GetAttrString = nullptr;
PyObject_HasAttr_t pPyObject_HasAttr = nullptr;
PyObject_GetAttr_t pPyObject_GetAttr = nullptr;
PyObject_SetAttrString_t pPyObject_SetAttrString = nullptr;
PyObject_SetAttr_t pPyObject_SetAttr = nullptr;
PyObject_CallObject_t pPyObject_CallObject = nullptr;
PyObject_Call_t pPyObject_Call = nullptr;
PyObject_CallNoArgs_t pPyObject_CallNoArgs = nullptr;
PyObject_CallFunction_t pPyObject_CallFunction = nullptr;
PyObject_CallMethod_t pPyObject_CallMethod = nullptr;
PyObject_Type_t pPyObject_Type = nullptr;
PyObject_Size_t pPyObject_Size = nullptr;
PyObject_GetItem_t pPyObject_GetItem = nullptr;
PyObject_SetItem_t pPyObject_SetItem = nullptr;
PyObject_DelItem_t pPyObject_DelItem = nullptr;
PyObject_DelItemString_t pPyObject_DelItemString = nullptr;
PyObject_AsCharBuffer_t pPyObject_AsCharBuffer = nullptr;
PyObject_CheckReadBuffer_t pPyObject_CheckReadBuffer = nullptr;
PyObject_Format_t pPyObject_Format = nullptr;
PyObject_GetIter_t pPyObject_GetIter = nullptr;
PyObject_IsTrue_t pPyObject_IsTrue = nullptr;
PyObject_Not_t pPyObject_Not = nullptr;
PyCallable_Check_t pPyCallable_Check = nullptr;

PySequence_Check_t pPySequence_Check = nullptr;
PySequence_Size_t pPySequence_Size = nullptr;
PySequence_Concat_t pPySequence_Concat = nullptr;
PySequence_Repeat_t pPySequence_Repeat = nullptr;
PySequence_GetItem_t pPySequence_GetItem = nullptr;
PySequence_GetSlice_t pPySequence_GetSlice = nullptr;
PySequence_SetItem_t pPySequence_SetItem = nullptr;
PySequence_DelItem_t pPySequence_DelItem = nullptr;
PySequence_SetSlice_t pPySequence_SetSlice = nullptr;
PySequence_DelSlice_t pPySequence_DelSlice = nullptr;
PySequence_Tuple_t pPySequence_Tuple = nullptr;
PySequence_List_t pPySequence_List = nullptr;
PySequence_Fast_t pPySequence_Fast = nullptr;
PySequence_Count_t pPySequence_Count = nullptr;
PySequence_Contains_t pPySequence_Contains = nullptr;
PySequence_Index_t pPySequence_Index = nullptr;
PySequence_InPlaceConcat_t pPySequence_InPlaceConcat = nullptr;
PySequence_InPlaceRepeat_t pPySequence_InPlaceRepeat = nullptr;

PyImport_ImportModule_t pPyImport_ImportModule = nullptr;
PyImport_Import_t pPyImport_Import = nullptr;
PyImport_ReloadModule_t pPyImport_ReloadModule = nullptr;
PyImport_AddModule_t pPyImport_AddModule = nullptr;
PyImport_GetModuleDict_t pPyImport_GetModuleDict = nullptr;
PyImport_ImportModuleLevel_t pPyImport_ImportModuleLevel = nullptr;

_Py_Dealloc_t p_Py_Dealloc = nullptr;

PySys_GetObject_t pPySys_GetObject = nullptr;
PySys_SetObject_t pPySys_SetObject = nullptr;
PySys_WriteStdout_t pPySys_WriteStdout = nullptr;
PySys_WriteStderr_t pPySys_WriteStderr = nullptr;
PySys_FormatStdout_t pPySys_FormatStdout = nullptr;
PySys_FormatStderr_t pPySys_FormatStderr = nullptr;
PySys_ResetWarnOptions_t pPySys_ResetWarnOptions = nullptr;
PySys_GetXOptions_t pPySys_GetXOptions = nullptr;

Py_CompileString_t pPy_CompileString = nullptr;
PyRun_SimpleString_t pPyRun_SimpleString = nullptr;
PyErr_PrintEx_t pPyErr_PrintEx = nullptr;
PyErr_Display_t pPyErr_Display = nullptr;

// Initialize type pointers to nullptr
PyObject* pPyType_Type = nullptr;
PyObject* pPyBaseObject_Type = nullptr;
PyObject* pPySuper_Type = nullptr;
PyObject* pPyBool_Type = nullptr;
PyObject* pPyFloat_Type = nullptr;
PyObject* pPyLong_Type = nullptr;
PyObject* pPyList_Type = nullptr;
PyObject* pPyDict_Type = nullptr;
PyObject* pPyUnicode_Type = nullptr;
PyObject* pPyBytes_Type = nullptr;
PyObject* pPyExc_RuntimeError = nullptr;
PyObject* pPyExc_ValueError = nullptr;
PyObject* pPyProperty_Type = nullptr;
PyObject* pPyTuple_Type = nullptr;
PyObject* pPy_None = nullptr;


PyObject_CallFunctionObjArgs_t pPyObject_CallFunctionObjArgs = nullptr;
PyUnicode_Join_t pPyUnicode_Join = nullptr;
PyObject_RichCompareBool_t pPyObject_RichCompareBool = nullptr;



PyUnicode_DecodeUTF8_t pPyUnicode_DecodeUTF8 = nullptr;
PyUnicode_AsEncodedString_t pPyUnicode_AsEncodedString = nullptr;
PyUnicode_Decode_t pPyUnicode_Decode = nullptr;
PyUnicode_FromEncodedObject_t pPyUnicode_FromEncodedObject = nullptr;
PyUnicode_GetLength_t pPyUnicode_GetLength = nullptr;
PyUnicode_FromKindAndData_t pPyUnicode_FromKindAndData = nullptr;
PyUnicode_AsUTF16String_t pPyUnicode_AsUTF16String = nullptr;
PyUnicode_AsUTF32String_t pPyUnicode_AsUTF32String = nullptr;
PyModule_GetDict_t pPyModule_GetDict = nullptr;
PyUnicode_AsUTF8String_t pPyUnicode_AsUTF8String = nullptr;

PyErr_NormalizeException_t pPyErr_NormalizeException = nullptr;
PyObject_Repr_t pPyObject_Repr = nullptr;

PyObject_IsInstance_t pPyObject_IsInstance = nullptr;

PyInterpreterState_Get_t pPyInterpreterState_Get = nullptr;

#ifdef _WIN32
static HMODULE python_lib_handle = nullptr;

std::string GetLastErrorAsString()
{
	DWORD error = GetLastError();
	if(error == 0) return "";

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(
	        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	        NULL,
	        error,
	        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	        (LPSTR)&messageBuffer,
	        0,
	        NULL);

	std::string message(messageBuffer, size);
	LocalFree(messageBuffer);
	return message;
}
#else
static void* python_lib_handle = nullptr;

std::vector<std::string> get_python_search_paths()
{
	std::vector<std::string> search_paths;

	// Common installation paths
	search_paths.push_back("/usr/lib/");
	search_paths.push_back("/usr/local/lib/");
	search_paths.push_back("/usr/lib/x86_64-linux-gnu/");

	// Add paths from environment variables
	const char* ld_library_path = getenv("LD_LIBRARY_PATH");
	if(ld_library_path)
	{
		std::string paths(ld_library_path);
		size_t pos = 0;
		while((pos = paths.find(':')) != std::string::npos)
		{
			search_paths.push_back(paths.substr(0, pos) + "/");
			paths.erase(0, pos + 1);
		}
		if(!paths.empty())
		{
			search_paths.push_back(paths + "/");
		}
	}

	// Add Python-specific environment variables
	const char* python_home = getenv("PYTHONHOME");
	if(python_home)
	{
		search_paths.push_back(std::string(python_home) + "/lib/");
	}

	// Try to get Python library path from python3-config if available
	FILE* pipe = popen("python3-config --ldflags 2>/dev/null", "r");
	if(pipe)
	{
		char buffer[256];
		std::string result;
		while(fgets(buffer, sizeof(buffer), pipe) != NULL)
		{
			result += buffer;
		}
		pclose(pipe);

		// Parse the output for -L flags
		size_t pos = 0;
		while((pos = result.find("-L", pos)) != std::string::npos)
		{
			pos += 2;// Skip "-L"
			size_t end = result.find(' ', pos);
			if(end != std::string::npos)
			{
				search_paths.push_back(result.substr(pos, end - pos) + "/");
			}
		}
	}

	return search_paths;
}
#endif


bool load_python3_api()
{
#ifdef _WIN32
	// First try to find already loaded Python DLL
	HMODULE hModule = nullptr;
	if(EnumProcessModules(GetCurrentProcess(), &hModule, sizeof(hModule), nullptr))
	{
		char moduleName[MAX_PATH];
		if(GetModuleFileNameA(hModule, moduleName, sizeof(moduleName)))
		{
			std::string name(moduleName);
			if(name.find("python3") != std::string::npos || name.find("python311") != std::string::npos)
			{
				python_lib_handle = hModule;
				printf("Found already loaded Python DLL: %s\n", name.c_str());
			}
		}
	}

	// If not found, try loading it
	if(!python_lib_handle)
	{
		const char* lib_names[] = {
		        "python311.dll",
		        "python3.dll",
		};

		std::string last_error;
		for(const char* lib_name: lib_names)
		{
			python_lib_handle = LoadLibraryA(lib_name);
			if(python_lib_handle) break;
			last_error = GetLastErrorAsString();
		}

		if(!python_lib_handle)
		{
			std::stringstream ss;
			ss << "Failed to load Python DLL. ";
			if(!last_error.empty())
			{
				ss << "Last error: " << last_error;
			}
			throw std::runtime_error(ss.str());
		}
	}

#define LOAD_SYMBOL(handle, symbol, type)                                    \
	p##symbol = (type)GetProcAddress(handle, #symbol);                   \
	if(!p##symbol)                                                                \
	{                                                                       \
		std::string err = GetLastErrorAsString();                           \
		throw std::runtime_error(std::string("Failed to load ") + #symbol + \
		                         (err.empty() ? "" : ": " + err));          \
	}

#else
	// First try to find symbols in the global scope (from the running Python interpreter)
	void* test_symbol = dlsym(RTLD_DEFAULT, "Py_Initialize");
	if(test_symbol) {
		printf("Found Python symbols in global scope, using RTLD_DEFAULT\n");
		fflush(stdout);
		python_lib_handle = RTLD_DEFAULT;
	}
	else {
		printf("No Python symbols found in global scope, trying to load library explicitly\n");
		fflush(stdout);
		
		// Try to find already loaded Python library
		struct dl_phdr_info info = {0};
		Dl_info dl_info;

		dl_iterate_phdr([](struct dl_phdr_info* info, size_t size, void* data) -> int {
			if (info->dlpi_name && (strstr(info->dlpi_name, "libpython3.11") || strstr(info->dlpi_name, "libpython3"))) {
				void** handle = static_cast<void**>(data);
				// Get handle to already loaded library without loading it again
				*handle = dlopen(info->dlpi_name, RTLD_NOLOAD | RTLD_NOW);
				if (*handle) {
					printf("Found already loaded Python library: %s\n", info->dlpi_name);
					fflush(stdout);
					return 1; // Stop iteration
				}
			}
			return 0; // Continue iteration
		}, &python_lib_handle);

		// If not found, try loading it
		if(!python_lib_handle)
		{
			printf("+++ didn't find already loaded python library\n");
			
			const char* lib_names[] = {
					"libpython3.11.so",
					"libpython3.11.so.1.0",
					"libpython3.11.so.1",
					"libpython3.so",
			};

			std::string load_errors;

			// First try loading directly (using system's library search paths)
			for(const char* lib_name: lib_names)
			{
				python_lib_handle = dlopen(lib_name, RTLD_NOW | RTLD_GLOBAL);
				if(python_lib_handle)
				{
					char path[4096] = {0};
					if (dlinfo(python_lib_handle, RTLD_DI_ORIGIN, path) == 0) {
						printf("Loaded Python library from: %s/%s\n", path, lib_name);
					}
					else {
						printf("Loaded Python library (path unknown): %s\n", lib_name);
					}
					break;
				}
				load_errors += std::string("\nTried ") + lib_name + ": " + dlerror();
			}

			// If direct loading failed, try explicit paths
			if(!python_lib_handle)
			{
				std::vector<std::string> search_paths = get_python_search_paths();
				for(const auto& path: search_paths)
				{
					for(const char* lib_name: lib_names)
					{
						std::string full_path = path + lib_name;
						python_lib_handle = dlopen(full_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
						if(python_lib_handle)
						{
							printf("Loaded Python library from path: %s\n", full_path.c_str());
							break;
						}
						load_errors += std::string("\nTried ") + full_path + ": " + dlerror();
					}
					if(python_lib_handle) break;
				}
			}

			if(!python_lib_handle)
			{
				throw std::runtime_error("Failed to load Python library. Attempted paths:" + load_errors);
			}
		}
	}

#define LOAD_SYMBOL(handle, symbol, type)         \
	p##symbol = (type)dlsym(handle, #symbol); \
	if(!p##symbol) throw std::runtime_error(std::string("Failed to load ") + #symbol + ": " + dlerror())

#endif

	// Load all the function pointers with their correct symbol names
	LOAD_SYMBOL(python_lib_handle, PyEval_SaveThread, PyEval_SaveThread_t);
	LOAD_SYMBOL(python_lib_handle, PyEval_RestoreThread, PyEval_RestoreThread_t);
	LOAD_SYMBOL(python_lib_handle, PyGILState_Ensure, pPyGILState_Ensure_t);
	LOAD_SYMBOL(python_lib_handle, PyGILState_Release, PyGILState_Release_t);
	LOAD_SYMBOL(python_lib_handle, PyGILState_GetThisThreadState, PyGILState_GetThisThreadState_t);
	LOAD_SYMBOL(python_lib_handle, PyGILState_Check, PyGILState_Check_t);

	LOAD_SYMBOL(python_lib_handle, Py_Initialize, Py_Initialize_t);
	LOAD_SYMBOL(python_lib_handle, Py_InitializeEx, Py_InitializeEx_t);
	LOAD_SYMBOL(python_lib_handle, Py_Finalize, Py_Finalize_t);
	LOAD_SYMBOL(python_lib_handle, Py_FinalizeEx, Py_FinalizeEx_t);
	LOAD_SYMBOL(python_lib_handle, Py_IsInitialized, Py_IsInitialized_t);

	LOAD_SYMBOL(python_lib_handle, PyUnicode_FromString, PyUnicode_FromString_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_FromStringAndSize, PyUnicode_FromStringAndSize_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_FromFormat, PyUnicode_FromFormat_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF8, PyUnicode_AsUTF8_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF8AndSize, PyUnicode_AsUTF8AndSize_t);

	LOAD_SYMBOL(python_lib_handle, PyLong_FromLong, PyLong_FromLong_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_FromUnsignedLong, PyLong_FromUnsignedLong_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_FromDouble, PyLong_FromDouble_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_FromString, PyLong_FromString_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_FromLongLong, PyLong_FromLongLong_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_FromUnsignedLongLong, PyLong_FromUnsignedLongLong_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_AsLong, PyLong_AsLong_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_AsLongLong, PyLong_AsLongLong_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_AsUnsignedLong, PyLong_AsUnsignedLong_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_AsUnsignedLongLong, PyLong_AsUnsignedLongLong_t);

	LOAD_SYMBOL(python_lib_handle, PyFloat_FromDouble, PyFloat_FromDouble_t);
	LOAD_SYMBOL(python_lib_handle, PyFloat_FromString, PyFloat_FromString_t);
	LOAD_SYMBOL(python_lib_handle, PyFloat_AsDouble, PyFloat_AsDouble_t);

	LOAD_SYMBOL(python_lib_handle, PyList_New, PyList_New_t);
	LOAD_SYMBOL(python_lib_handle, PyList_Size, PyList_Size_t);
	LOAD_SYMBOL(python_lib_handle, PyList_GetItem, PyList_GetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyList_SetItem, PyList_SetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyList_Insert, PyList_Insert_t);
	LOAD_SYMBOL(python_lib_handle, PyList_Append, PyList_Append_t);
	LOAD_SYMBOL(python_lib_handle, PyList_GetSlice, PyList_GetSlice_t);
	LOAD_SYMBOL(python_lib_handle, PyList_SetSlice, PyList_SetSlice_t);

	LOAD_SYMBOL(python_lib_handle, PyTuple_New, PyTuple_New_t);
	LOAD_SYMBOL(python_lib_handle, PyTuple_Size, PyTuple_Size_t);
	LOAD_SYMBOL(python_lib_handle, PyTuple_GetItem, PyTuple_GetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyTuple_SetItem, PyTuple_SetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyTuple_GetSlice, PyTuple_GetSlice_t);

	LOAD_SYMBOL(python_lib_handle, PyDict_New, PyDict_New_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_SetItem, PyDict_SetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_SetItemString, PyDict_SetItemString_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_GetItem, PyDict_GetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_GetItemString, PyDict_GetItemString_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_DelItem, PyDict_DelItem_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_DelItemString, PyDict_DelItemString_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_Clear, PyDict_Clear_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_Next, PyDict_Next_t);
	LOAD_SYMBOL(python_lib_handle, PyDict_Size, PyDict_Size_t);
	
	LOAD_SYMBOL(python_lib_handle, PyErr_SetString, PyErr_SetString_t);
	
	LOAD_SYMBOL(python_lib_handle, PyErr_SetObject, PyErr_SetObject_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_Occurred, PyErr_Occurred_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_Clear, PyErr_Clear_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_Print, PyErr_Print_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_WriteUnraisable, PyErr_WriteUnraisable_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_ExceptionMatches, PyErr_ExceptionMatches_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_GivenExceptionMatches, PyErr_GivenExceptionMatches_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_Fetch, PyErr_Fetch_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_Restore, PyErr_Restore_t);

	LOAD_SYMBOL(python_lib_handle, PyObject_Print, PyObject_Print_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_HasAttrString, PyObject_HasAttrString_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_GetAttrString, PyObject_GetAttrString_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_HasAttr, PyObject_HasAttr_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_GetAttr, PyObject_GetAttr_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_SetAttrString, PyObject_SetAttrString_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_SetAttr, PyObject_SetAttr_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_CallObject, PyObject_CallObject_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_Call, PyObject_Call_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_CallNoArgs, PyObject_CallNoArgs_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_CallFunction, PyObject_CallFunction_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_CallMethod, PyObject_CallMethod_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_Type, PyObject_Type_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_Size, PyObject_Size_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_GetItem, PyObject_GetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_SetItem, PyObject_SetItem_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_DelItem, PyObject_DelItem_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_DelItemString, PyObject_DelItemString_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_AsCharBuffer, PyObject_AsCharBuffer_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_CheckReadBuffer, PyObject_CheckReadBuffer_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_Format, PyObject_Format_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_GetIter, PyObject_GetIter_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_IsTrue, PyObject_IsTrue_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_Not, PyObject_Not_t);
	LOAD_SYMBOL(python_lib_handle, PyCallable_Check, PyCallable_Check_t);

	LOAD_SYMBOL(python_lib_handle, PySequence_Check, PySequence_Check_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Size, PySequence_Size_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Concat, PySequence_Concat_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Repeat, PySequence_Repeat_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_GetItem, PySequence_GetItem_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_GetSlice, PySequence_GetSlice_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_SetItem, PySequence_SetItem_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_DelItem, PySequence_DelItem_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_SetSlice, PySequence_SetSlice_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_DelSlice, PySequence_DelSlice_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Tuple, PySequence_Tuple_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_List, PySequence_List_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Fast, PySequence_Fast_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Count, PySequence_Count_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Contains, PySequence_Contains_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_Index, PySequence_Index_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_InPlaceConcat, PySequence_InPlaceConcat_t);
	LOAD_SYMBOL(python_lib_handle, PySequence_InPlaceRepeat, PySequence_InPlaceRepeat_t);

	LOAD_SYMBOL(python_lib_handle, PyImport_ImportModule, PyImport_ImportModule_t);
	LOAD_SYMBOL(python_lib_handle, PyImport_Import, PyImport_Import_t);
	LOAD_SYMBOL(python_lib_handle, PyImport_ReloadModule, PyImport_ReloadModule_t);
	LOAD_SYMBOL(python_lib_handle, PyImport_AddModule, PyImport_AddModule_t);
	LOAD_SYMBOL(python_lib_handle, PyImport_GetModuleDict, PyImport_GetModuleDict_t);
	LOAD_SYMBOL(python_lib_handle, PyImport_ImportModuleLevel, PyImport_ImportModuleLevel_t);

	LOAD_SYMBOL(python_lib_handle, _Py_Dealloc, _Py_Dealloc_t);

	// Load PySys functions
	LOAD_SYMBOL(python_lib_handle, PySys_GetObject, PySys_GetObject_t);
	LOAD_SYMBOL(python_lib_handle, PySys_SetObject, PySys_SetObject_t);
	LOAD_SYMBOL(python_lib_handle, PySys_WriteStdout, PySys_WriteStdout_t);
	LOAD_SYMBOL(python_lib_handle, PySys_WriteStderr, PySys_WriteStderr_t);
	LOAD_SYMBOL(python_lib_handle, PySys_FormatStdout, PySys_FormatStdout_t);
	LOAD_SYMBOL(python_lib_handle, PySys_FormatStderr, PySys_FormatStderr_t);
	LOAD_SYMBOL(python_lib_handle, PySys_ResetWarnOptions, PySys_ResetWarnOptions_t);
	LOAD_SYMBOL(python_lib_handle, PySys_GetXOptions, PySys_GetXOptions_t);

	// Load pythonrun.h functions
	LOAD_SYMBOL(python_lib_handle, Py_CompileString, Py_CompileString_t);
	LOAD_SYMBOL(python_lib_handle, PyRun_SimpleString, PyRun_SimpleString_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_PrintEx, PyErr_PrintEx_t);
	LOAD_SYMBOL(python_lib_handle, PyErr_Display, PyErr_Display_t);

	// Load the new functions
	LOAD_SYMBOL(python_lib_handle, PyBool_FromLong, PyBool_FromLong_t);
	LOAD_SYMBOL(python_lib_handle, PyBytes_FromString, PyBytes_FromString_t);
	LOAD_SYMBOL(python_lib_handle, PyBytes_FromStringAndSize, PyBytes_FromStringAndSize_t);
	LOAD_SYMBOL(python_lib_handle, PyBytes_Size, PyBytes_Size_t);
	LOAD_SYMBOL(python_lib_handle, PyBytes_AsString, PyBytes_AsString_t);
	LOAD_SYMBOL(python_lib_handle, PyBytes_AsStringAndSize, PyBytes_AsStringAndSize_t);

	LOAD_SYMBOL(python_lib_handle, PyLong_AsVoidPtr, PyLong_AsVoidPtr_t);
	LOAD_SYMBOL(python_lib_handle, PyLong_FromVoidPtr, PyLong_FromVoidPtr_t);

	LOAD_SYMBOL(python_lib_handle, PyMapping_GetItemString, PyMapping_GetItemString_t);

	LOAD_SYMBOL(python_lib_handle, PyUnicode_DecodeUTF8, PyUnicode_DecodeUTF8_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_AsEncodedString, PyUnicode_AsEncodedString_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_Decode, PyUnicode_Decode_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_FromEncodedObject, PyUnicode_FromEncodedObject_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_GetLength, PyUnicode_GetLength_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_FromKindAndData, PyUnicode_FromKindAndData_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF16String, PyUnicode_AsUTF16String_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF32String, PyUnicode_AsUTF32String_t);

	LOAD_SYMBOL(python_lib_handle, PyErr_NormalizeException, PyErr_NormalizeException_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_Repr, PyObject_Repr_t);

	// Load the new functions
	LOAD_SYMBOL(python_lib_handle, PyObject_CallFunctionObjArgs, PyObject_CallFunctionObjArgs_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_Join,PyUnicode_Join_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_RichCompareBool, PyObject_RichCompareBool_t);
	
	// Load new function pointers
	LOAD_SYMBOL(python_lib_handle, PyObject_CallFunctionObjArgs, PyObject_CallFunctionObjArgs_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_Join, PyUnicode_Join_t);
	LOAD_SYMBOL(python_lib_handle, PyObject_RichCompareBool, PyObject_RichCompareBool_t);

	LOAD_SYMBOL(python_lib_handle, PyModule_GetDict, PyModule_GetDict_t);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF8String, PyUnicode_AsUTF8String_t);

	LOAD_SYMBOL(python_lib_handle, PyObject_IsInstance, PyObject_IsInstance_t);

	LOAD_SYMBOL(python_lib_handle, PyInterpreterState_Get, PyInterpreterState_Get_t);

#ifdef _WIN32 // variables in Linux are loaded differently due to GOT.
	LOAD_SYMBOL(python_lib_handle, PyExc_RuntimeError, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyExc_ValueError, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyProperty_Type, PyTypeObject*);

	
	// Load type pointers (updated names)
	LOAD_SYMBOL(python_lib_handle, PyType_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyBaseObject_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PySuper_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyBool_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyFloat_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyLong_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyTuple_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyList_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyDict_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyUnicode_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyBytes_Type, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyExc_RuntimeError, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyExc_ValueError, PyTypeObject*);
	LOAD_SYMBOL(python_lib_handle, PyProperty_Type, PyTypeObject*);

	// Load Py_None
#ifdef _WIN32
	pPy_None = (PyTypeObject*)GetProcAddress(python_lib_handle, "_Py_NoneStruct");
	if(!pPy_None)
	{
		std::string err = GetLastErrorAsString();
		throw std::runtime_error(std::string("Failed to load ") + "_Py_NoneStruct" + (err.empty() ? "" : ": " + err));
	}
#else
	pPy_None = (PyObject*)dlsym(python_lib_handle, "_Py_NoneStruct");
	if(!pPy_None) throw std::runtime_error(std::string("Failed to load ") + "Py_None" + ": " + dlerror());
#endif

#endif
#undef LOAD_SYMBOL

	
	return true;
}

#ifndef _WIN32
void load_python3_variables_from_interpreter()
{
    // Create a Python dictionary containing all the required types
    const char* setup_code = R"(
import sys
import builtins

type_dict = {
    'PyType_Type': type,
    'PyBaseObject_Type': object,
    'PySuper_Type': super,
    'PyBool_Type': bool,
    'PyFloat_Type': float,
    'PyLong_Type': int,
    'PyList_Type': list,
    'PyDict_Type': dict,
    'PyUnicode_Type': str,
    'PyBytes_Type': bytes,
    'PyExc_RuntimeError': RuntimeError,
    'PyExc_ValueError': ValueError,
    'PyProperty_Type': property,
    'PyTuple_Type': tuple,
    'Py_None': None
}

# Store in sys module for access from C
sys.type_dict = type_dict
)";

	
    // Execute the Python code to set up the dictionary
    if (pPyRun_SimpleString(setup_code) != 0) {
        throw std::runtime_error("Failed to execute Python setup code");
    }
	
    // Get the sys module to access our dictionary
    PyObject* sys_module = pPySys_GetObject("modules");
    if (!sys_module) {
        throw std::runtime_error("Failed to get sys.modules");
    }
	
    PyObject* sys = pPyDict_GetItemString(sys_module, "sys");
    if (!sys) {
        throw std::runtime_error("Failed to get sys module");
    }
	
    PyObject* type_dict = pPyObject_GetAttrString(sys, "type_dict");
    if (!type_dict) {
        throw std::runtime_error("Failed to get type_dict");
    }

    // Helper lambda to get a type from the dictionary
    auto get_type = [type_dict](const char* name) -> PyObject* {
		
        PyObject* type = pPyDict_GetItemString(type_dict, name);
        if (!type) {
            throw std::runtime_error(std::string("Failed to get ") + name);
        }
		
        return type;
    };

	
	
    // Load all the type variables
    pPyType_Type = get_type("PyType_Type");
    pPyBaseObject_Type = get_type("PyBaseObject_Type");
    pPySuper_Type = get_type("PySuper_Type");
    pPyBool_Type = get_type("PyBool_Type");
    pPyFloat_Type = get_type("PyFloat_Type");
    pPyLong_Type = get_type("PyLong_Type");
    pPyList_Type = get_type("PyList_Type");
    pPyDict_Type = get_type("PyDict_Type");
    pPyUnicode_Type = get_type("PyUnicode_Type");
    pPyBytes_Type = get_type("PyBytes_Type");
    pPyExc_RuntimeError = get_type("PyExc_RuntimeError");
    pPyExc_ValueError = get_type("PyExc_ValueError");
    pPyProperty_Type = get_type("PyProperty_Type");
    pPyTuple_Type = get_type("PyTuple_Type");
    pPy_None = get_type("Py_None");
	
    // Clean up the temporary dictionary from sys module
    if (pPyRun_SimpleString("del sys.type_dict") != 0) {
        throw std::runtime_error("Failed to clean up type_dict");
    }
}
#endif