#pragma once

#include <string>
#include "python_h_declares.h"

// Function pointer types for Python API functions
typedef PyThreadState* (*PyEval_SaveThread_t)(void);
typedef void (*PyEval_RestoreThread_t)(PyThreadState *tstate);
typedef PyGILState_STATE (*PyGILState_Ensure_t)(void);
typedef void (*PyGILState_Release_t)(PyGILState_STATE);
typedef PyThreadState* (*PyGILState_GetThisThreadState_t)(void);
typedef int (*PyGILState_Check_t)(void);

typedef void (*Py_Initialize_t)(void);
typedef void (*Py_InitializeEx_t)(int initsigs);
typedef void (*Py_Finalize_t)(void);
typedef int (*Py_FinalizeEx_t)(void);
typedef int (*Py_IsInitialized_t)(void);

typedef PyObject* (*PyUnicode_FromString_t)(const char *u);
typedef PyObject* (*PyUnicode_FromStringAndSize_t)(const char *u, Py_ssize_t size);
typedef PyObject* (*PyUnicode_FromFormat_t)(const char *format, ...);
typedef const char* (*PyUnicode_AsUTF8_t)(PyObject *unicode);
typedef const char* (*PyUnicode_AsUTF8AndSize_t)(PyObject *unicode, Py_ssize_t *size);

typedef PyObject* (*PyLong_FromLong_t)(long);
typedef PyObject* (*PyLong_FromUnsignedLong_t)(unsigned long);
typedef PyObject* (*PyLong_FromDouble_t)(double);
typedef PyObject* (*PyLong_FromString_t)(const char *str, char **pend, int base);
typedef long (*PyLong_AsLong_t)(PyObject *);
typedef long long (*PyLong_AsLongLong_t)(PyObject *);
typedef unsigned long (*PyLong_AsUnsignedLong_t)(PyObject *);
typedef unsigned long long (*PyLong_AsUnsignedLongLong_t)(PyObject *);

typedef PyObject* (*PyFloat_FromDouble_t)(double);
typedef PyObject* (*PyFloat_FromString_t)(PyObject*);
typedef double (*PyFloat_AsDouble_t)(PyObject *);

typedef PyObject* (*PyList_New_t)(Py_ssize_t size);
typedef Py_ssize_t (*PyList_Size_t)(PyObject *);
typedef PyObject* (*PyList_GetItem_t)(PyObject *, Py_ssize_t);
typedef int (*PyList_SetItem_t)(PyObject *, Py_ssize_t, PyObject *);
typedef int (*PyList_Insert_t)(PyObject *, Py_ssize_t, PyObject *);
typedef int (*PyList_Append_t)(PyObject *, PyObject *);
typedef PyObject* (*PyList_GetSlice_t)(PyObject *, Py_ssize_t, Py_ssize_t);
typedef int (*PyList_SetSlice_t)(PyObject *, Py_ssize_t, Py_ssize_t, PyObject *);

typedef PyObject* (*PyTuple_New_t)(Py_ssize_t size);
typedef Py_ssize_t (*PyTuple_Size_t)(PyObject *);
typedef PyObject* (*PyTuple_GetItem_t)(PyObject *, Py_ssize_t);
typedef int (*PyTuple_SetItem_t)(PyObject *, Py_ssize_t, PyObject *);
typedef PyObject* (*PyTuple_GetSlice_t)(PyObject *, Py_ssize_t, Py_ssize_t);

typedef PyObject* (*PyDict_New_t)(void);
typedef int (*PyDict_SetItem_t)(PyObject *mp, PyObject *key, PyObject *item);
typedef int (*PyDict_SetItemString_t)(PyObject *dp, const char *key, PyObject *item);
typedef PyObject* (*PyDict_GetItem_t)(PyObject *mp, PyObject *key);
typedef PyObject* (*PyDict_GetItemString_t)(PyObject *dp, const char *key);
typedef int (*PyDict_DelItem_t)(PyObject *mp, PyObject *key);
typedef int (*PyDict_DelItemString_t)(PyObject *dp, const char *key);
typedef int (*PyDict_Clear_t)(PyObject *mp);
typedef int (*PyDict_Next_t)(PyObject *mp, Py_ssize_t *pos, PyObject **key, PyObject **value);
typedef Py_ssize_t (*PyDict_Size_t)(PyObject *mp);

typedef void (*PyErr_SetString_t)(PyObject *type, const char *message);
typedef void (*PyErr_SetObject_t)(PyObject *type, PyObject *value);
typedef PyObject* (*PyErr_Occurred_t)(void);
typedef void (*PyErr_Clear_t)(void);
typedef void (*PyErr_Print_t)(void);
typedef void (*PyErr_WriteUnraisable_t)(PyObject *obj);
typedef int (*PyErr_ExceptionMatches_t)(PyObject *exc);
typedef int (*PyErr_GivenExceptionMatches_t)(PyObject *given, PyObject *exc);
typedef void (*PyErr_Fetch_t)(PyObject **ptype, PyObject **pvalue, PyObject **ptraceback);
typedef void (*PyErr_Restore_t)(PyObject *type, PyObject *value, PyObject *traceback);

typedef int (*PyObject_Print_t)(PyObject *o, FILE *fp, int flags);
typedef int (*PyObject_HasAttrString_t)(PyObject *o, const char *attr_name);
typedef PyObject* (*PyObject_GetAttrString_t)(PyObject *o, const char *attr_name);
typedef int (*PyObject_HasAttr_t)(PyObject *o, PyObject *attr_name);
typedef PyObject* (*PyObject_GetAttr_t)(PyObject *o, PyObject *attr_name);
typedef int (*PyObject_SetAttrString_t)(PyObject *o, const char *attr_name, PyObject *v);
typedef int (*PyObject_SetAttr_t)(PyObject *o, PyObject *attr_name, PyObject *v);
typedef PyObject* (*PyObject_CallObject_t)(PyObject *callable, PyObject *args);
typedef PyObject* (*PyObject_Call_t)(PyObject *callable, PyObject *args, PyObject *kwargs);
typedef PyObject* (*PyObject_CallNoArgs_t)(PyObject *func);
typedef PyObject* (*PyObject_CallFunction_t)(PyObject *callable, const char *format, ...);
typedef PyObject* (*PyObject_CallMethod_t)(PyObject *obj, const char *name, const char *format, ...);
typedef PyObject* (*PyObject_Type_t)(PyObject *o);
typedef Py_ssize_t (*PyObject_Size_t)(PyObject *o);
typedef PyObject* (*PyObject_GetItem_t)(PyObject *o, PyObject *key);
typedef int (*PyObject_SetItem_t)(PyObject *o, PyObject *key, PyObject *v);
typedef int (*PyObject_DelItem_t)(PyObject *o, PyObject *key);
typedef int (*PyObject_DelItemString_t)(PyObject *o, const char *key);
typedef int (*PyObject_AsCharBuffer_t)(PyObject *obj, const char **buffer, Py_ssize_t *buffer_len);
typedef int (*PyObject_CheckReadBuffer_t)(PyObject *obj);
typedef PyObject* (*PyObject_Format_t)(PyObject* obj, PyObject* format_spec);
typedef PyObject* (*PyObject_GetIter_t)(PyObject *o);
typedef int (*PyObject_IsTrue_t)(PyObject *o);
typedef int (*PyObject_Not_t)(PyObject *o);
typedef int (*PyCallable_Check_t)(PyObject *o);

typedef int (*PySequence_Check_t)(PyObject *o);
typedef Py_ssize_t (*PySequence_Size_t)(PyObject *o);
typedef PyObject* (*PySequence_Concat_t)(PyObject *o1, PyObject *o2);
typedef PyObject* (*PySequence_Repeat_t)(PyObject *o, Py_ssize_t count);
typedef PyObject* (*PySequence_GetItem_t)(PyObject *o, Py_ssize_t i);
typedef PyObject* (*PySequence_GetSlice_t)(PyObject *o, Py_ssize_t i1, Py_ssize_t i2);
typedef int (*PySequence_SetItem_t)(PyObject *o, Py_ssize_t i, PyObject *v);
typedef int (*PySequence_DelItem_t)(PyObject *o, Py_ssize_t i);
typedef int (*PySequence_SetSlice_t)(PyObject *o, Py_ssize_t i1, Py_ssize_t i2, PyObject *v);
typedef int (*PySequence_DelSlice_t)(PyObject *o, Py_ssize_t i1, Py_ssize_t i2);
typedef PyObject* (*PySequence_Tuple_t)(PyObject *o);
typedef PyObject* (*PySequence_List_t)(PyObject *o);
typedef PyObject* (*PySequence_Fast_t)(PyObject *o, const char* m);
typedef Py_ssize_t (*PySequence_Count_t)(PyObject *o, PyObject *value);
typedef int (*PySequence_Contains_t)(PyObject *seq, PyObject *ob);
typedef Py_ssize_t (*PySequence_Index_t)(PyObject *o, PyObject *value);
typedef PyObject* (*PySequence_InPlaceConcat_t)(PyObject *o1, PyObject *o2);
typedef PyObject* (*PySequence_InPlaceRepeat_t)(PyObject *o, Py_ssize_t count);

typedef PyObject* (*PyImport_ImportModule_t)(const char *name);
typedef PyObject* (*PyImport_ImportModuleEx_t)(const char *name, PyObject *globals, PyObject *locals, PyObject *fromlist);
typedef PyObject* (*PyImport_Import_t)(PyObject *name);
typedef PyObject* (*PyImport_ReloadModule_t)(PyObject *m);
typedef PyObject* (*PyImport_AddModule_t)(const char *name);
typedef PyObject* (*PyImport_GetModuleDict_t)(void);

typedef void (*_Py_Dealloc_t)(PyObject *obj);

typedef PyObject* (*PySys_GetObject_t)(const char *);
typedef int (*PySys_SetObject_t)(const char *, PyObject *);
typedef void (*PySys_WriteStdout_t)(const char *format, ...);
typedef void (*PySys_WriteStderr_t)(const char *format, ...);
typedef void (*PySys_FormatStdout_t)(const char *format, ...);
typedef void (*PySys_FormatStderr_t)(const char *format, ...);
typedef void (*PySys_ResetWarnOptions_t)(void);
typedef PyObject* (*PySys_GetXOptions_t)(void);

typedef PyObject* (*Py_CompileString_t)(const char *, const char *, int);
typedef int (*PyRun_SimpleString_t)(const char *);
typedef void (*PyErr_PrintEx_t)(int);
typedef void (*PyErr_Display_t)(PyObject *, PyObject *, PyObject *);

typedef PyObject* (*PyBool_FromLong_t)(long);
typedef PyObject* (*PyBytes_FromString_t)(const char *);
typedef PyObject* (*PyBytes_FromStringAndSize_t)(const char *, Py_ssize_t);
typedef Py_ssize_t (*PyBytes_Size_t)(PyObject *);
typedef char* (*PyBytes_AsString_t)(PyObject *);
typedef int (*PyBytes_AsStringAndSize_t)(PyObject *, char **, Py_ssize_t *);

typedef void* (*PyLong_AsVoidPtr_t)(PyObject *);
typedef PyObject* (*PyLong_FromVoidPtr_t)(void *);

typedef PyObject* (*PyMapping_GetItemString_t)(PyObject *o, const char *key);

typedef PyObject* (*PyUnicode_DecodeUTF8_t)(const char *s, Py_ssize_t size, const char *errors);
typedef PyObject* (*PyUnicode_AsEncodedString_t)(PyObject *unicode, const char *encoding, const char *errors);
typedef PyObject* (*PyUnicode_Decode_t)(const char *s, Py_ssize_t size, const char *encoding, const char *errors);
typedef PyObject* (*PyUnicode_FromEncodedObject_t)(PyObject *obj, const char *encoding, const char *errors);
typedef Py_ssize_t (*PyUnicode_GetLength_t)(PyObject *unicode);
typedef PyObject* (*PyUnicode_FromKindAndData_t)(int kind, const void *buffer, Py_ssize_t size);
typedef PyObject* (*PyUnicode_AsUTF16String_t)(PyObject *unicode);
typedef PyObject* (*PyUnicode_AsUTF32String_t)(PyObject *unicode);

typedef void (*PyErr_NormalizeException_t)(PyObject**, PyObject**, PyObject**);
typedef PyObject* (*PyObject_Repr_t)(PyObject *);

// Extern declarations of function pointers
extern PyEval_SaveThread_t PyEval_SaveThread;
extern PyEval_RestoreThread_t PyEval_RestoreThread;
extern PyGILState_Ensure_t PyGILState_Ensure;
extern PyGILState_Release_t PyGILState_Release;
extern PyGILState_GetThisThreadState_t PyGILState_GetThisThreadState;
extern PyGILState_Check_t PyGILState_Check;

extern Py_Initialize_t Py_Initialize;
extern Py_InitializeEx_t Py_InitializeEx;
extern Py_Finalize_t Py_Finalize;
extern Py_FinalizeEx_t Py_FinalizeEx;
extern Py_IsInitialized_t Py_IsInitialized;

extern PyUnicode_FromString_t PyUnicode_FromString;
extern PyUnicode_FromStringAndSize_t PyUnicode_FromStringAndSize;
extern PyUnicode_FromFormat_t PyUnicode_FromFormat;
extern PyUnicode_AsUTF8_t PyUnicode_AsUTF8;
extern PyUnicode_AsUTF8AndSize_t PyUnicode_AsUTF8AndSize;

extern PyLong_FromLong_t PyLong_FromLong;
extern PyLong_FromUnsignedLong_t PyLong_FromUnsignedLong;
extern PyLong_FromDouble_t PyLong_FromDouble;
extern PyLong_FromString_t PyLong_FromString;
extern PyLong_AsLong_t PyLong_AsLong;
extern PyLong_AsLongLong_t PyLong_AsLongLong;
extern PyLong_AsUnsignedLong_t PyLong_AsUnsignedLong;
extern PyLong_AsUnsignedLongLong_t PyLong_AsUnsignedLongLong;

extern PyFloat_FromDouble_t PyFloat_FromDouble;
extern PyFloat_FromString_t PyFloat_FromString;
extern PyFloat_AsDouble_t PyFloat_AsDouble;

extern PyList_New_t PyList_New;
extern PyList_Size_t PyList_Size;
extern PyList_GetItem_t PyList_GetItem;
extern PyList_SetItem_t PyList_SetItem;
extern PyList_Insert_t PyList_Insert;
extern PyList_Append_t PyList_Append;
extern PyList_GetSlice_t PyList_GetSlice;
extern PyList_SetSlice_t PyList_SetSlice;

extern PyTuple_New_t PyTuple_New;
extern PyTuple_Size_t PyTuple_Size;
extern PyTuple_GetItem_t PyTuple_GetItem;
extern PyTuple_SetItem_t PyTuple_SetItem;
extern PyTuple_GetSlice_t PyTuple_GetSlice;

extern PyDict_New_t PyDict_New;
extern PyDict_SetItem_t PyDict_SetItem;
extern PyDict_SetItemString_t PyDict_SetItemString;
extern PyDict_GetItem_t PyDict_GetItem;
extern PyDict_GetItemString_t PyDict_GetItemString;
extern PyDict_DelItem_t PyDict_DelItem;
extern PyDict_DelItemString_t PyDict_DelItemString;
extern PyDict_Clear_t PyDict_Clear;
extern PyDict_Next_t PyDict_Next;
extern PyDict_Size_t PyDict_Size;

extern PyErr_SetString_t PyErr_SetString;
extern PyErr_SetObject_t PyErr_SetObject;
extern PyErr_Occurred_t PyErr_Occurred;
extern PyErr_Clear_t PyErr_Clear;
extern PyErr_Print_t PyErr_Print;
extern PyErr_WriteUnraisable_t PyErr_WriteUnraisable;
extern PyErr_ExceptionMatches_t PyErr_ExceptionMatches;
extern PyErr_GivenExceptionMatches_t PyErr_GivenExceptionMatches;
extern PyErr_Fetch_t PyErr_Fetch;
extern PyErr_Restore_t PyErr_Restore;

extern PyObject_Print_t PyObject_Print;
extern PyObject_HasAttrString_t PyObject_HasAttrString;
extern PyObject_GetAttrString_t PyObject_GetAttrString;
extern PyObject_HasAttr_t PyObject_HasAttr;
extern PyObject_GetAttr_t PyObject_GetAttr;
extern PyObject_SetAttrString_t PyObject_SetAttrString;
extern PyObject_SetAttr_t PyObject_SetAttr;
extern PyObject_CallObject_t PyObject_CallObject;
extern PyObject_Call_t PyObject_Call;
extern PyObject_CallNoArgs_t PyObject_CallNoArgs;
extern PyObject_CallFunction_t PyObject_CallFunction;
extern PyObject_CallMethod_t PyObject_CallMethod;
extern PyObject_Type_t PyObject_Type;
extern PyObject_Size_t PyObject_Size;
extern PyObject_GetItem_t PyObject_GetItem;
extern PyObject_SetItem_t PyObject_SetItem;
extern PyObject_DelItem_t PyObject_DelItem;
extern PyObject_DelItemString_t PyObject_DelItemString;
extern PyObject_AsCharBuffer_t PyObject_AsCharBuffer;
extern PyObject_CheckReadBuffer_t PyObject_CheckReadBuffer;
extern PyObject_Format_t PyObject_Format;
extern PyObject_GetIter_t PyObject_GetIter;
extern PyObject_IsTrue_t PyObject_IsTrue;
extern PyObject_Not_t PyObject_Not;
extern PyCallable_Check_t PyCallable_Check;

extern PySequence_Check_t PySequence_Check;
extern PySequence_Size_t PySequence_Size;
extern PySequence_Concat_t PySequence_Concat;
extern PySequence_Repeat_t PySequence_Repeat;
extern PySequence_GetItem_t PySequence_GetItem;
extern PySequence_GetSlice_t PySequence_GetSlice;
extern PySequence_SetItem_t PySequence_SetItem;
extern PySequence_DelItem_t PySequence_DelItem;
extern PySequence_SetSlice_t PySequence_SetSlice;
extern PySequence_DelSlice_t PySequence_DelSlice;
extern PySequence_Tuple_t PySequence_Tuple;
extern PySequence_List_t PySequence_List;
extern PySequence_Fast_t PySequence_Fast;
extern PySequence_Count_t PySequence_Count;
extern PySequence_Contains_t PySequence_Contains;
extern PySequence_Index_t PySequence_Index;
extern PySequence_InPlaceConcat_t PySequence_InPlaceConcat;
extern PySequence_InPlaceRepeat_t PySequence_InPlaceRepeat;

extern PyImport_ImportModule_t PyImport_ImportModule;
extern PyImport_ImportModuleEx_t PyImport_ImportModuleEx;
extern PyImport_Import_t PyImport_Import;
extern PyImport_ReloadModule_t PyImport_ReloadModule;
extern PyImport_AddModule_t PyImport_AddModule;
extern PyImport_GetModuleDict_t PyImport_GetModuleDict;

extern _Py_Dealloc_t _Py_Dealloc;

extern PySys_GetObject_t PySys_GetObject;
extern PySys_SetObject_t PySys_SetObject;
extern PySys_WriteStdout_t PySys_WriteStdout;
extern PySys_WriteStderr_t PySys_WriteStderr;
extern PySys_FormatStdout_t PySys_FormatStdout;
extern PySys_FormatStderr_t PySys_FormatStderr;
extern PySys_ResetWarnOptions_t PySys_ResetWarnOptions;
extern PySys_GetXOptions_t PySys_GetXOptions;

extern Py_CompileString_t Py_CompileString;
extern PyRun_SimpleString_t PyRun_SimpleString;
extern PyErr_PrintEx_t PyErr_PrintEx;
extern PyErr_Display_t PyErr_Display;

extern PyBool_FromLong_t PyBool_FromLong;
extern PyBytes_FromString_t PyBytes_FromString;
extern PyBytes_FromStringAndSize_t PyBytes_FromStringAndSize;
extern PyBytes_Size_t PyBytes_Size;
extern PyBytes_AsString_t PyBytes_AsString;
extern PyBytes_AsStringAndSize_t PyBytes_AsStringAndSize;

extern PyLong_AsVoidPtr_t PyLong_AsVoidPtr;
extern PyLong_FromVoidPtr_t PyLong_FromVoidPtr;

extern PyMapping_GetItemString_t PyMapping_GetItemString;

extern PyUnicode_DecodeUTF8_t PyUnicode_DecodeUTF8;
extern PyUnicode_AsEncodedString_t PyUnicode_AsEncodedString;
extern PyUnicode_Decode_t PyUnicode_Decode;
extern PyUnicode_FromEncodedObject_t PyUnicode_FromEncodedObject;
extern PyUnicode_GetLength_t PyUnicode_GetLength;
extern PyUnicode_FromKindAndData_t PyUnicode_FromKindAndData;
extern PyUnicode_AsUTF16String_t PyUnicode_AsUTF16String;
extern PyUnicode_AsUTF32String_t PyUnicode_AsUTF32String;

extern PyErr_NormalizeException_t PyErr_NormalizeException;
extern PyObject_Repr_t PyObject_Repr;

// Add type pointer declarations
extern PyTypeObject* PyType_Type_ptr;
extern PyTypeObject* PyBaseObject_Type_ptr;
extern PyTypeObject* PySuper_Type_ptr;
extern PyTypeObject* PyBool_Type_ptr;
extern PyTypeObject* PyFloat_Type_ptr;
extern PyTypeObject* PyLong_Type_ptr;
extern PyTypeObject* PyTuple_Type_ptr;
extern PyTypeObject* PyList_Type_ptr;
extern PyTypeObject* PyDict_Type_ptr;
extern PyTypeObject* PyUnicode_Type_ptr;
extern PyTypeObject* PyBytes_Type_ptr;
extern PyTypeObject* PyExc_RuntimeError_ptr;
extern PyTypeObject* PyProperty_Type_ptr;

// Add new function pointer types
typedef PyObject* (*PyObject_CallFunctionObjArgs_t)(PyObject *callable, ...);
typedef PyObject* (*PyUnicode_Join_t)(PyObject *separator, PyObject *seq);
typedef int (*PyObject_RichCompareBool_t)(PyObject *o1, PyObject *o2, int opid);

// Add new function pointer declarations
extern PyObject_CallFunctionObjArgs_t PyObject_CallFunctionObjArgs;
extern PyUnicode_Join_t PyUnicode_Join;
extern PyObject_RichCompareBool_t PyObject_RichCompareBool;

// Update type declarations (remove _ptr suffix)
extern PyTypeObject* PyType_Type;
extern PyTypeObject* PyBaseObject_Type;
extern PyTypeObject* PySuper_Type;
extern PyTypeObject* PyBool_Type;
extern PyTypeObject* PyFloat_Type;
extern PyTypeObject* PyLong_Type;
extern PyTypeObject* PyTuple_Type;
extern PyTypeObject* PyList_Type;
extern PyTypeObject* PyDict_Type;
extern PyTypeObject* PyUnicode_Type;
extern PyTypeObject* PyBytes_Type;
extern PyTypeObject* PyExc_RuntimeError;
extern PyTypeObject* PyExc_ValueError;
extern PyTypeObject* PyProperty_Type;

// Add PyBytes_AS_STRING macro
#define PyBytes_AS_STRING(op) (PyBytes_AsString(op))


// Add Py_IsNone macro
#define Py_IsNone(x) ((x) == Py_None)

// Function to load all Python API functions
bool load_python3_api();