#include "python3_api_wrapper.h"
#include <stdexcept>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <filesystem>
#endif

// Initialize all function pointers to nullptr
PyEval_SaveThread_t PyEval_SaveThread = nullptr;
PyEval_RestoreThread_t PyEval_RestoreThread = nullptr;
PyGILState_Ensure_t PyGILState_Ensure = nullptr;
PyGILState_Release_t PyGILState_Release = nullptr;
PyGILState_GetThisThreadState_t PyGILState_GetThisThreadState = nullptr;
PyGILState_Check_t PyGILState_Check = nullptr;

Py_Initialize_t Py_Initialize = nullptr;
Py_InitializeEx_t Py_InitializeEx = nullptr;
Py_Finalize_t Py_Finalize = nullptr;
Py_FinalizeEx_t Py_FinalizeEx = nullptr;
Py_IsInitialized_t Py_IsInitialized = nullptr;

PyUnicode_FromString_t PyUnicode_FromString = nullptr;
PyUnicode_FromStringAndSize_t PyUnicode_FromStringAndSize = nullptr;
PyUnicode_FromFormat_t PyUnicode_FromFormat = nullptr;
PyUnicode_AsUTF8_t PyUnicode_AsUTF8 = nullptr;
PyUnicode_AsUTF8AndSize_t PyUnicode_AsUTF8AndSize = nullptr;

PyLong_FromLong_t PyLong_FromLong = nullptr;
PyLong_FromUnsignedLong_t PyLong_FromUnsignedLong = nullptr;
PyLong_FromDouble_t PyLong_FromDouble = nullptr;
PyLong_FromString_t PyLong_FromString = nullptr;
PyLong_AsLong_t PyLong_AsLong = nullptr;
PyLong_AsLongLong_t PyLong_AsLongLong = nullptr;
PyLong_AsUnsignedLong_t PyLong_AsUnsignedLong = nullptr;
PyLong_AsUnsignedLongLong_t PyLong_AsUnsignedLongLong = nullptr;

PyFloat_FromDouble_t PyFloat_FromDouble = nullptr;
PyFloat_FromString_t PyFloat_FromString = nullptr;
PyFloat_AsDouble_t PyFloat_AsDouble = nullptr;

PyList_New_t PyList_New = nullptr;
PyList_Size_t PyList_Size = nullptr;
PyList_GetItem_t PyList_GetItem = nullptr;
PyList_SetItem_t PyList_SetItem = nullptr;
PyList_Insert_t PyList_Insert = nullptr;
PyList_Append_t PyList_Append = nullptr;
PyList_GetSlice_t PyList_GetSlice = nullptr;
PyList_SetSlice_t PyList_SetSlice = nullptr;

PyTuple_New_t PyTuple_New = nullptr;
PyTuple_Size_t PyTuple_Size = nullptr;
PyTuple_GetItem_t PyTuple_GetItem = nullptr;
PyTuple_SetItem_t PyTuple_SetItem = nullptr;
PyTuple_GetSlice_t PyTuple_GetSlice = nullptr;

PyDict_New_t PyDict_New = nullptr;
PyDict_SetItem_t PyDict_SetItem = nullptr;
PyDict_SetItemString_t PyDict_SetItemString = nullptr;
PyDict_GetItem_t PyDict_GetItem = nullptr;
PyDict_GetItemString_t PyDict_GetItemString = nullptr;
PyDict_DelItem_t PyDict_DelItem = nullptr;
PyDict_DelItemString_t PyDict_DelItemString = nullptr;
PyDict_Clear_t PyDict_Clear = nullptr;
PyDict_Next_t PyDict_Next = nullptr;
PyDict_Size_t PyDict_Size = nullptr;

PyErr_SetString_t PyErr_SetString = nullptr;
PyErr_SetObject_t PyErr_SetObject = nullptr;
PyErr_Occurred_t PyErr_Occurred = nullptr;
PyErr_Clear_t PyErr_Clear = nullptr;
PyErr_Print_t PyErr_Print = nullptr;
PyErr_WriteUnraisable_t PyErr_WriteUnraisable = nullptr;
PyErr_ExceptionMatches_t PyErr_ExceptionMatches = nullptr;
PyErr_GivenExceptionMatches_t PyErr_GivenExceptionMatches = nullptr;
PyErr_Fetch_t PyErr_Fetch = nullptr;
PyErr_Restore_t PyErr_Restore = nullptr;

PyObject_Print_t PyObject_Print = nullptr;
PyObject_HasAttrString_t PyObject_HasAttrString = nullptr;
PyObject_GetAttrString_t PyObject_GetAttrString = nullptr;
PyObject_HasAttr_t PyObject_HasAttr = nullptr;
PyObject_GetAttr_t PyObject_GetAttr = nullptr;
PyObject_SetAttrString_t PyObject_SetAttrString = nullptr;
PyObject_SetAttr_t PyObject_SetAttr = nullptr;
PyObject_CallObject_t PyObject_CallObject = nullptr;
PyObject_Call_t PyObject_Call = nullptr;
PyObject_CallNoArgs_t PyObject_CallNoArgs = nullptr;
PyObject_CallFunction_t PyObject_CallFunction = nullptr;
PyObject_CallMethod_t PyObject_CallMethod = nullptr;
PyObject_Type_t PyObject_Type = nullptr;
PyObject_Size_t PyObject_Size = nullptr;
PyObject_GetItem_t PyObject_GetItem = nullptr;
PyObject_SetItem_t PyObject_SetItem = nullptr;
PyObject_DelItem_t PyObject_DelItem = nullptr;
PyObject_DelItemString_t PyObject_DelItemString = nullptr;
PyObject_AsCharBuffer_t PyObject_AsCharBuffer = nullptr;
PyObject_CheckReadBuffer_t PyObject_CheckReadBuffer = nullptr;
PyObject_Format_t PyObject_Format = nullptr;
PyObject_GetIter_t PyObject_GetIter = nullptr;
PyObject_IsTrue_t PyObject_IsTrue = nullptr;
PyObject_Not_t PyObject_Not = nullptr;
PyCallable_Check_t PyCallable_Check = nullptr;

PySequence_Check_t PySequence_Check = nullptr;
PySequence_Size_t PySequence_Size = nullptr;
PySequence_Concat_t PySequence_Concat = nullptr;
PySequence_Repeat_t PySequence_Repeat = nullptr;
PySequence_GetItem_t PySequence_GetItem = nullptr;
PySequence_GetSlice_t PySequence_GetSlice = nullptr;
PySequence_SetItem_t PySequence_SetItem = nullptr;
PySequence_DelItem_t PySequence_DelItem = nullptr;
PySequence_SetSlice_t PySequence_SetSlice = nullptr;
PySequence_DelSlice_t PySequence_DelSlice = nullptr;
PySequence_Tuple_t PySequence_Tuple = nullptr;
PySequence_List_t PySequence_List = nullptr;
PySequence_Fast_t PySequence_Fast = nullptr;
PySequence_Count_t PySequence_Count = nullptr;
PySequence_Contains_t PySequence_Contains = nullptr;
PySequence_Index_t PySequence_Index = nullptr;
PySequence_InPlaceConcat_t PySequence_InPlaceConcat = nullptr;
PySequence_InPlaceRepeat_t PySequence_InPlaceRepeat = nullptr;

PyImport_ImportModule_t PyImport_ImportModule = nullptr;
PyImport_ImportModuleEx_t PyImport_ImportModuleEx = nullptr;
PyImport_Import_t PyImport_Import = nullptr;
PyImport_ReloadModule_t PyImport_ReloadModule = nullptr;
PyImport_AddModule_t PyImport_AddModule = nullptr;
PyImport_GetModuleDict_t PyImport_GetModuleDict = nullptr;

_Py_Dealloc_t _Py_Dealloc = nullptr;

PySys_GetObject_t PySys_GetObject = nullptr;
PySys_SetObject_t PySys_SetObject = nullptr;
PySys_WriteStdout_t PySys_WriteStdout = nullptr;
PySys_WriteStderr_t PySys_WriteStderr = nullptr;
PySys_FormatStdout_t PySys_FormatStdout = nullptr;
PySys_FormatStderr_t PySys_FormatStderr = nullptr;
PySys_ResetWarnOptions_t PySys_ResetWarnOptions = nullptr;
PySys_GetXOptions_t PySys_GetXOptions = nullptr;

Py_CompileString_t Py_CompileString = nullptr;
PyRun_SimpleString_t PyRun_SimpleString = nullptr;
PyErr_PrintEx_t PyErr_PrintEx = nullptr;
PyErr_Display_t PyErr_Display = nullptr;

// Initialize type pointers to nullptr
PyTypeObject* PyType_Type = nullptr;
PyTypeObject* PyBaseObject_Type = nullptr;
PyTypeObject* PySuper_Type = nullptr;
PyTypeObject* PyBool_Type = nullptr;
PyTypeObject* PyFloat_Type = nullptr;
PyTypeObject* PyLong_Type = nullptr;
PyTypeObject* PyTuple_Type = nullptr;
PyTypeObject* PyList_Type = nullptr;
PyTypeObject* PyDict_Type = nullptr;
PyTypeObject* PyUnicode_Type = nullptr;
PyTypeObject* PyBytes_Type = nullptr;
PyTypeObject* PyExc_RuntimeError = nullptr;
PyTypeObject* PyExc_ValueError = nullptr;
PyTypeObject* PyProperty_Type = nullptr;

// Initialize the new function pointers
PyObject_CallFunctionObjArgs_t PyObject_CallFunctionObjArgs = nullptr;
PyUnicode_Join_t PyUnicode_Join = nullptr;
PyObject_RichCompareBool_t PyObject_RichCompareBool = nullptr;

// Initialize Py_None
PyObject* Py_None = nullptr;

PyUnicode_DecodeUTF8_t PyUnicode_DecodeUTF8 = nullptr;
PyUnicode_AsEncodedString_t PyUnicode_AsEncodedString = nullptr;
PyUnicode_Decode_t PyUnicode_Decode = nullptr;
PyUnicode_FromEncodedObject_t PyUnicode_FromEncodedObject = nullptr;
PyUnicode_GetLength_t PyUnicode_GetLength = nullptr;
PyUnicode_FromKindAndData_t PyUnicode_FromKindAndData = nullptr;
PyUnicode_AsUTF16String_t PyUnicode_AsUTF16String = nullptr;
PyUnicode_AsUTF32String_t PyUnicode_AsUTF32String = nullptr;

#ifdef _WIN32
static HMODULE python_lib_handle = nullptr;

std::string GetLastErrorAsString() {
    DWORD error = GetLastError();
    if (error == 0) return "";

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}
#else
static void* python_lib_handle = nullptr;

std::vector<std::string> get_python_search_paths() {
    std::vector<std::string> search_paths;
    
    // Common installation paths
    search_paths.push_back("/usr/lib/");
    search_paths.push_back("/usr/local/lib/");
    search_paths.push_back("/usr/lib/x86_64-linux-gnu/");
    
    // Add paths from environment variables
    const char* ld_library_path = getenv("LD_LIBRARY_PATH");
    if (ld_library_path) {
        std::string paths(ld_library_path);
        size_t pos = 0;
        while ((pos = paths.find(':')) != std::string::npos) {
            search_paths.push_back(paths.substr(0, pos) + "/");
            paths.erase(0, pos + 1);
        }
        if (!paths.empty()) {
            search_paths.push_back(paths + "/");
        }
    }
    
    // Add Python-specific environment variables
    const char* python_home = getenv("PYTHONHOME");
    if (python_home) {
        search_paths.push_back(std::string(python_home) + "/lib/");
    }
    
    // Try to get Python library path from python3-config if available
    FILE* pipe = popen("python3-config --ldflags 2>/dev/null", "r");
    if (pipe) {
        char buffer[256];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
        pclose(pipe);
        
        // Parse the output for -L flags
        size_t pos = 0;
        while ((pos = result.find("-L", pos)) != std::string::npos) {
            pos += 2;  // Skip "-L"
            size_t end = result.find(' ', pos);
            if (end != std::string::npos) {
                search_paths.push_back(result.substr(pos, end - pos) + "/");
            }
        }
    }
    
    return search_paths;
}
#endif

bool load_python3_api() {
    #ifdef _WIN32
        // Try common Python DLL names on Windows
        const char* lib_names[] = {
            "python311.dll",
            "python3.dll",
            // Add more potential DLL names if needed
        };
        
        std::string last_error;
        for (const char* lib_name : lib_names) {
            python_lib_handle = LoadLibraryA(lib_name);
            if (python_lib_handle) break;
            last_error = GetLastErrorAsString();
        }
        
        if (!python_lib_handle) {
            std::stringstream ss;
            ss << "Failed to load Python DLL. ";
            if (!last_error.empty()) {
                ss << "Last error: " << last_error;
            }
            throw std::runtime_error(ss.str());
        }

        #define LOAD_SYMBOL(handle, var, symbol) \
            var = (decltype(var))GetProcAddress(handle, #symbol); \
            if (!var) { \
                std::string err = GetLastErrorAsString(); \
                throw std::runtime_error(std::string("Failed to load ") + #symbol + \
                    (err.empty() ? "" : ": " + err)); \
            }

    #else
        // Try common Python SO names on Linux/Unix
        const char* lib_names[] = {
            "libpython3.11.so",
            "libpython3.11.so.1.0",
            "libpython3.11.so.1",
            "libpython3.so",
            // Add more potential SO names if needed
        };
        
        std::vector<std::string> search_paths = get_python_search_paths();
        std::string load_errors;
        
        // First try loading directly (using system's library search paths)
        for (const char* lib_name : lib_names) {
            python_lib_handle = dlopen(lib_name, RTLD_LAZY);
            if (python_lib_handle) break;
            load_errors += std::string("\nTried ") + lib_name + ": " + dlerror();
        }
        
        // If direct loading failed, try explicit paths
        if (!python_lib_handle) {
            for (const auto& path : search_paths) {
                for (const char* lib_name : lib_names) {
                    std::string full_path = path + lib_name;
                    python_lib_handle = dlopen(full_path.c_str(), RTLD_LAZY);
                    if (python_lib_handle) break;
                    load_errors += std::string("\nTried ") + full_path + ": " + dlerror();
                }
                if (python_lib_handle) break;
            }
        }
        
        if (!python_lib_handle) {
            throw std::runtime_error("Failed to load Python library. Attempted paths:" + load_errors);
        }

    #define LOAD_SYMBOL(handle, var, symbol) \
            var = (decltype(var))dlsym(handle, #symbol); \
            if (!var) throw std::runtime_error(std::string("Failed to load ") + #symbol + ": " + dlerror())
    #endif

    // Load all the function pointers with their correct symbol names
    LOAD_SYMBOL(python_lib_handle, PyEval_SaveThread, PyEval_SaveThread);
    LOAD_SYMBOL(python_lib_handle, PyEval_RestoreThread, PyEval_RestoreThread);
    LOAD_SYMBOL(python_lib_handle, PyGILState_Ensure, PyGILState_Ensure);
    LOAD_SYMBOL(python_lib_handle, PyGILState_Release, PyGILState_Release);
    LOAD_SYMBOL(python_lib_handle, PyGILState_GetThisThreadState, PyGILState_GetThisThreadState);
    LOAD_SYMBOL(python_lib_handle, PyGILState_Check, PyGILState_Check);

    LOAD_SYMBOL(python_lib_handle, Py_Initialize, Py_Initialize);
    LOAD_SYMBOL(python_lib_handle, Py_InitializeEx, Py_InitializeEx);
    LOAD_SYMBOL(python_lib_handle, Py_Finalize, Py_Finalize);
    LOAD_SYMBOL(python_lib_handle, Py_FinalizeEx, Py_FinalizeEx);
    LOAD_SYMBOL(python_lib_handle, Py_IsInitialized, Py_IsInitialized);

    LOAD_SYMBOL(python_lib_handle, PyUnicode_FromString, PyUnicode_FromString);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_FromStringAndSize, PyUnicode_FromStringAndSize);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_FromFormat, PyUnicode_FromFormat);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF8, PyUnicode_AsUTF8);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF8AndSize, PyUnicode_AsUTF8AndSize);

    LOAD_SYMBOL(python_lib_handle, PyLong_FromLong, PyLong_FromLong);
    LOAD_SYMBOL(python_lib_handle, PyLong_FromUnsignedLong, PyLong_FromUnsignedLong);
    LOAD_SYMBOL(python_lib_handle, PyLong_FromDouble, PyLong_FromDouble);
    LOAD_SYMBOL(python_lib_handle, PyLong_FromString, PyLong_FromString);
    LOAD_SYMBOL(python_lib_handle, PyLong_AsLong, PyLong_AsLong);
    LOAD_SYMBOL(python_lib_handle, PyLong_AsLongLong, PyLong_AsLongLong);
    LOAD_SYMBOL(python_lib_handle, PyLong_AsUnsignedLong, PyLong_AsUnsignedLong);
    LOAD_SYMBOL(python_lib_handle, PyLong_AsUnsignedLongLong, PyLong_AsUnsignedLongLong);

    LOAD_SYMBOL(python_lib_handle, PyFloat_FromDouble, PyFloat_FromDouble);
    LOAD_SYMBOL(python_lib_handle, PyFloat_FromString, PyFloat_FromString);
    LOAD_SYMBOL(python_lib_handle, PyFloat_AsDouble, PyFloat_AsDouble);

    LOAD_SYMBOL(python_lib_handle, PyList_New, PyList_New);
    LOAD_SYMBOL(python_lib_handle, PyList_Size, PyList_Size);
    LOAD_SYMBOL(python_lib_handle, PyList_GetItem, PyList_GetItem);
    LOAD_SYMBOL(python_lib_handle, PyList_SetItem, PyList_SetItem);
    LOAD_SYMBOL(python_lib_handle, PyList_Insert, PyList_Insert);
    LOAD_SYMBOL(python_lib_handle, PyList_Append, PyList_Append);
    LOAD_SYMBOL(python_lib_handle, PyList_GetSlice, PyList_GetSlice);
    LOAD_SYMBOL(python_lib_handle, PyList_SetSlice, PyList_SetSlice);

    LOAD_SYMBOL(python_lib_handle, PyTuple_New, PyTuple_New);
    LOAD_SYMBOL(python_lib_handle, PyTuple_Size, PyTuple_Size);
    LOAD_SYMBOL(python_lib_handle, PyTuple_GetItem, PyTuple_GetItem);
    LOAD_SYMBOL(python_lib_handle, PyTuple_SetItem, PyTuple_SetItem);
    LOAD_SYMBOL(python_lib_handle, PyTuple_GetSlice, PyTuple_GetSlice);

    LOAD_SYMBOL(python_lib_handle, PyDict_New, PyDict_New);
    LOAD_SYMBOL(python_lib_handle, PyDict_SetItem, PyDict_SetItem);
    LOAD_SYMBOL(python_lib_handle, PyDict_SetItemString, PyDict_SetItemString);
    LOAD_SYMBOL(python_lib_handle, PyDict_GetItem, PyDict_GetItem);
    LOAD_SYMBOL(python_lib_handle, PyDict_GetItemString, PyDict_GetItemString);
    LOAD_SYMBOL(python_lib_handle, PyDict_DelItem, PyDict_DelItem);
    LOAD_SYMBOL(python_lib_handle, PyDict_DelItemString, PyDict_DelItemString);
    LOAD_SYMBOL(python_lib_handle, PyDict_Clear, PyDict_Clear);
    LOAD_SYMBOL(python_lib_handle, PyDict_Next, PyDict_Next);
    LOAD_SYMBOL(python_lib_handle, PyDict_Size, PyDict_Size);

    LOAD_SYMBOL(python_lib_handle, PyErr_SetString, PyErr_SetString);
    LOAD_SYMBOL(python_lib_handle, PyErr_SetObject, PyErr_SetObject);
    LOAD_SYMBOL(python_lib_handle, PyErr_Occurred, PyErr_Occurred);
    LOAD_SYMBOL(python_lib_handle, PyErr_Clear, PyErr_Clear);
    LOAD_SYMBOL(python_lib_handle, PyErr_Print, PyErr_Print);
    LOAD_SYMBOL(python_lib_handle, PyErr_WriteUnraisable, PyErr_WriteUnraisable);
    LOAD_SYMBOL(python_lib_handle, PyErr_ExceptionMatches, PyErr_ExceptionMatches);
    LOAD_SYMBOL(python_lib_handle, PyErr_GivenExceptionMatches, PyErr_GivenExceptionMatches);
    LOAD_SYMBOL(python_lib_handle, PyErr_Fetch, PyErr_Fetch);
    LOAD_SYMBOL(python_lib_handle, PyErr_Restore, PyErr_Restore);

    LOAD_SYMBOL(python_lib_handle, PyObject_Print, PyObject_Print);
    LOAD_SYMBOL(python_lib_handle, PyObject_HasAttrString, PyObject_HasAttrString);
    LOAD_SYMBOL(python_lib_handle, PyObject_GetAttrString, PyObject_GetAttrString);
    LOAD_SYMBOL(python_lib_handle, PyObject_HasAttr, PyObject_HasAttr);
    LOAD_SYMBOL(python_lib_handle, PyObject_GetAttr, PyObject_GetAttr);
    LOAD_SYMBOL(python_lib_handle, PyObject_SetAttrString, PyObject_SetAttrString);
    LOAD_SYMBOL(python_lib_handle, PyObject_SetAttr, PyObject_SetAttr);
    LOAD_SYMBOL(python_lib_handle, PyObject_CallObject, PyObject_CallObject);
    LOAD_SYMBOL(python_lib_handle, PyObject_Call, PyObject_Call);
    LOAD_SYMBOL(python_lib_handle, PyObject_CallNoArgs, PyObject_CallNoArgs);
    LOAD_SYMBOL(python_lib_handle, PyObject_CallFunction, PyObject_CallFunction);
    LOAD_SYMBOL(python_lib_handle, PyObject_CallMethod, PyObject_CallMethod);
    LOAD_SYMBOL(python_lib_handle, PyObject_Type, PyObject_Type);
    LOAD_SYMBOL(python_lib_handle, PyObject_Size, PyObject_Size);
    LOAD_SYMBOL(python_lib_handle, PyObject_GetItem, PyObject_GetItem);
    LOAD_SYMBOL(python_lib_handle, PyObject_SetItem, PyObject_SetItem);
    LOAD_SYMBOL(python_lib_handle, PyObject_DelItem, PyObject_DelItem);
    LOAD_SYMBOL(python_lib_handle, PyObject_DelItemString, PyObject_DelItemString);
    LOAD_SYMBOL(python_lib_handle, PyObject_AsCharBuffer, PyObject_AsCharBuffer);
    LOAD_SYMBOL(python_lib_handle, PyObject_CheckReadBuffer, PyObject_CheckReadBuffer);
    LOAD_SYMBOL(python_lib_handle, PyObject_Format, PyObject_Format);
    LOAD_SYMBOL(python_lib_handle, PyObject_GetIter, PyObject_GetIter);
    LOAD_SYMBOL(python_lib_handle, PyObject_IsTrue, PyObject_IsTrue);
    LOAD_SYMBOL(python_lib_handle, PyObject_Not, PyObject_Not);
    LOAD_SYMBOL(python_lib_handle, PyCallable_Check, PyCallable_Check);

    LOAD_SYMBOL(python_lib_handle, PySequence_Check, PySequence_Check);
    LOAD_SYMBOL(python_lib_handle, PySequence_Size, PySequence_Size);
    LOAD_SYMBOL(python_lib_handle, PySequence_Concat, PySequence_Concat);
    LOAD_SYMBOL(python_lib_handle, PySequence_Repeat, PySequence_Repeat);
    LOAD_SYMBOL(python_lib_handle, PySequence_GetItem, PySequence_GetItem);
    LOAD_SYMBOL(python_lib_handle, PySequence_GetSlice, PySequence_GetSlice);
    LOAD_SYMBOL(python_lib_handle, PySequence_SetItem, PySequence_SetItem);
    LOAD_SYMBOL(python_lib_handle, PySequence_DelItem, PySequence_DelItem);
    LOAD_SYMBOL(python_lib_handle, PySequence_SetSlice, PySequence_SetSlice);
    LOAD_SYMBOL(python_lib_handle, PySequence_DelSlice, PySequence_DelSlice);
    LOAD_SYMBOL(python_lib_handle, PySequence_Tuple, PySequence_Tuple);
    LOAD_SYMBOL(python_lib_handle, PySequence_List, PySequence_List);
    LOAD_SYMBOL(python_lib_handle, PySequence_Fast, PySequence_Fast);
    LOAD_SYMBOL(python_lib_handle, PySequence_Count, PySequence_Count);
    LOAD_SYMBOL(python_lib_handle, PySequence_Contains, PySequence_Contains);
    LOAD_SYMBOL(python_lib_handle, PySequence_Index, PySequence_Index);
    LOAD_SYMBOL(python_lib_handle, PySequence_InPlaceConcat, PySequence_InPlaceConcat);
    LOAD_SYMBOL(python_lib_handle, PySequence_InPlaceRepeat, PySequence_InPlaceRepeat);

    LOAD_SYMBOL(python_lib_handle, PyImport_ImportModule, PyImport_ImportModule);
    LOAD_SYMBOL(python_lib_handle, PyImport_ImportModuleEx, PyImport_ImportModuleEx);
    LOAD_SYMBOL(python_lib_handle, PyImport_Import, PyImport_Import);
    LOAD_SYMBOL(python_lib_handle, PyImport_ReloadModule, PyImport_ReloadModule);
    LOAD_SYMBOL(python_lib_handle, PyImport_AddModule, PyImport_AddModule);
    LOAD_SYMBOL(python_lib_handle, PyImport_GetModuleDict, PyImport_GetModuleDict);

    LOAD_SYMBOL(python_lib_handle, _Py_Dealloc, _Py_Dealloc);

    // Load PySys functions
    LOAD_SYMBOL(python_lib_handle, PySys_GetObject, PySys_GetObject);
    LOAD_SYMBOL(python_lib_handle, PySys_SetObject, PySys_SetObject);
    LOAD_SYMBOL(python_lib_handle, PySys_WriteStdout, PySys_WriteStdout);
    LOAD_SYMBOL(python_lib_handle, PySys_WriteStderr, PySys_WriteStderr);
    LOAD_SYMBOL(python_lib_handle, PySys_FormatStdout, PySys_FormatStdout);
    LOAD_SYMBOL(python_lib_handle, PySys_FormatStderr, PySys_FormatStderr);
    LOAD_SYMBOL(python_lib_handle, PySys_ResetWarnOptions, PySys_ResetWarnOptions);
    LOAD_SYMBOL(python_lib_handle, PySys_GetXOptions, PySys_GetXOptions);

    // Load pythonrun.h functions
    LOAD_SYMBOL(python_lib_handle, Py_CompileString, Py_CompileString);
    LOAD_SYMBOL(python_lib_handle, PyRun_SimpleString, PyRun_SimpleString);
    LOAD_SYMBOL(python_lib_handle, PyErr_PrintEx, PyErr_PrintEx);
    LOAD_SYMBOL(python_lib_handle, PyErr_Display, PyErr_Display);

    // Load type pointers
    LOAD_SYMBOL(python_lib_handle, PyType_Type, PyType_Type);
    LOAD_SYMBOL(python_lib_handle, PyBaseObject_Type, PyBaseObject_Type);
    LOAD_SYMBOL(python_lib_handle, PySuper_Type, PySuper_Type);
    LOAD_SYMBOL(python_lib_handle, PyBool_Type, PyBool_Type);
    LOAD_SYMBOL(python_lib_handle, PyFloat_Type, PyFloat_Type);
    LOAD_SYMBOL(python_lib_handle, PyLong_Type, PyLong_Type);
    LOAD_SYMBOL(python_lib_handle, PyTuple_Type, PyTuple_Type);
    LOAD_SYMBOL(python_lib_handle, PyList_Type, PyList_Type);
    LOAD_SYMBOL(python_lib_handle, PyDict_Type, PyDict_Type);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_Type, PyUnicode_Type);
    LOAD_SYMBOL(python_lib_handle, PyBytes_Type, PyBytes_Type);

    // Load the new functions
    LOAD_SYMBOL(python_lib_handle, PyBool_FromLong, PyBool_FromLong);
    LOAD_SYMBOL(python_lib_handle, PyBytes_FromString, PyBytes_FromString);
    LOAD_SYMBOL(python_lib_handle, PyBytes_FromStringAndSize, PyBytes_FromStringAndSize);
    LOAD_SYMBOL(python_lib_handle, PyBytes_Size, PyBytes_Size);
    LOAD_SYMBOL(python_lib_handle, PyBytes_AsString, PyBytes_AsString);
    LOAD_SYMBOL(python_lib_handle, PyBytes_AsStringAndSize, PyBytes_AsStringAndSize);

    LOAD_SYMBOL(python_lib_handle, PyLong_AsVoidPtr, PyLong_AsVoidPtr);
    LOAD_SYMBOL(python_lib_handle, PyLong_FromVoidPtr, PyLong_FromVoidPtr);

    LOAD_SYMBOL(python_lib_handle, PyMapping_GetItemString, PyMapping_GetItemString);

    LOAD_SYMBOL(python_lib_handle, PyUnicode_DecodeUTF8, PyUnicode_DecodeUTF8);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_AsEncodedString, PyUnicode_AsEncodedString);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_Decode, PyUnicode_Decode);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_FromEncodedObject, PyUnicode_FromEncodedObject);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_GetLength, PyUnicode_GetLength);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_FromKindAndData, PyUnicode_FromKindAndData);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF16String, PyUnicode_AsUTF16String);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_AsUTF32String, PyUnicode_AsUTF32String);

    LOAD_SYMBOL(python_lib_handle, PyErr_NormalizeException, PyErr_NormalizeException);
    LOAD_SYMBOL(python_lib_handle, PyObject_Repr, PyObject_Repr);

    // Load the new types
    LOAD_SYMBOL(python_lib_handle, PyExc_RuntimeError, PyExc_RuntimeError);
    LOAD_SYMBOL(python_lib_handle, PyExc_ValueError, PyExc_ValueError);
    LOAD_SYMBOL(python_lib_handle, PyProperty_Type, PyProperty_Type);

    // Load the new functions
    LOAD_SYMBOL(python_lib_handle, PyObject_CallFunctionObjArgs, PyObject_CallFunctionObjArgs);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_Join, PyUnicode_Join);
    LOAD_SYMBOL(python_lib_handle, PyObject_RichCompareBool, PyObject_RichCompareBool);

    // Load type pointers (updated names)
    LOAD_SYMBOL(python_lib_handle, PyType_Type, PyType_Type);
    LOAD_SYMBOL(python_lib_handle, PyBaseObject_Type, PyBaseObject_Type);
    LOAD_SYMBOL(python_lib_handle, PySuper_Type, PySuper_Type);
    LOAD_SYMBOL(python_lib_handle, PyBool_Type, PyBool_Type);
    LOAD_SYMBOL(python_lib_handle, PyFloat_Type, PyFloat_Type);
    LOAD_SYMBOL(python_lib_handle, PyLong_Type, PyLong_Type);
    LOAD_SYMBOL(python_lib_handle, PyTuple_Type, PyTuple_Type);
    LOAD_SYMBOL(python_lib_handle, PyList_Type, PyList_Type);
    LOAD_SYMBOL(python_lib_handle, PyDict_Type, PyDict_Type);
    LOAD_SYMBOL(python_lib_handle, PyUnicode_Type, PyUnicode_Type);
    LOAD_SYMBOL(python_lib_handle, PyBytes_Type, PyBytes_Type);
    LOAD_SYMBOL(python_lib_handle, PyExc_RuntimeError, PyExc_RuntimeError);
    LOAD_SYMBOL(python_lib_handle, PyExc_ValueError, PyExc_ValueError);
    LOAD_SYMBOL(python_lib_handle, PyProperty_Type, PyProperty_Type);

    // Load Py_None
    LOAD_SYMBOL(python_lib_handle, Py_None, _Py_NoneStruct);

    #undef LOAD_SYMBOL

    return true;
}