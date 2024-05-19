import ctypes
from ctypes import *
from .metaffi_types import *
import platform
import os


def get_dynamic_lib_path_from_metaffi_home(fname: str):
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
	os.add_dll_directory(os.getenv('METAFFI_HOME') + '\\bin\\')

xllr_python3 = ctypes.cdll.LoadLibrary(get_dynamic_lib_path_from_metaffi_home('xllr.python311'))
xllr_python3.call_xcall.argtypes = [ctypes.c_void_p, ctypes.py_object, ctypes.py_object, ctypes.py_object]
xllr_python3.call_xcall.restype = ctypes.py_object

_xllr = cdll.LoadLibrary(get_dynamic_lib_path_from_metaffi_home('xllr'))

# Set argtypes and restype
_xllr.load_runtime_plugin.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
_xllr.load_runtime_plugin.restype = None

_xllr.free_runtime_plugin.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
_xllr.free_runtime_plugin.restype = None

_xllr.load_entity.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(metaffi_type_info), ctypes.c_int8, ctypes.POINTER(metaffi_type_info), ctypes.c_int8, ctypes.POINTER(ctypes.c_char_p)]
_xllr.load_entity.restype = ctypes.c_void_p

_xllr.free_xcall.argtypes = [ctypes.c_char_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p)]
_xllr.free_xcall.restype = None

_xllr.make_callable.argtypes = [ctypes.c_char_p, ctypes.py_object, ctypes.POINTER(metaffi_type_info), ctypes.c_int8, ctypes.POINTER(metaffi_type_info), ctypes.c_int8, ctypes.POINTER(ctypes.c_char_p)]
_xllr.make_callable.restype = ctypes.c_void_p

_xllr.alloc_cdts_buffer.argtypes = [ctypes.c_uint64, ctypes.c_uint64]
_xllr.alloc_cdts_buffer.restype = ctypes.c_void_p

_xllr.free_cdts_buffer.argtypes = [ctypes.c_void_p]
_xllr.free_cdts_buffer.restype = None

_xllr.free_string.argtypes = [ctypes.c_char_p]
_xllr.free_string.restype = None

if platform.system() == 'Windows':
	os.add_dll_directory(os.environ['METAFFI_HOME'])
	os.add_dll_directory(os.environ['METAFFI_HOME'] + '\\bin')


def load_runtime_plugin(runtime_plugin: str) -> None:
	global _xllr
	err = ctypes.c_char_p()
	_xllr.load_runtime_plugin(runtime_plugin.encode('utf-8'), ctypes.byref(err))
	
	# check if err is not NULL
	if err.value is not None:
		msg = err.value.decode('utf-8')
		_xllr.free_string(err)  # call xllr.free_string to free the memory allocated by xllr
		raise RuntimeError(msg)


def free_runtime_plugin(runtime_plugin: str) -> None:
	err = ctypes.c_char_p()
	_xllr.free_runtime_plugin(runtime_plugin.encode('utf-8'), ctypes.byref(err))
	
	# check if err is not NULL
	if err.value is not None:
		msg = err.value.decode('utf-8')
		_xllr.free_string(err)  # call xllr.free_string to free the memory allocated by xllr
		raise RuntimeError(msg)


def load_entity(runtime_plugin_name: str, module_path: str, function_path: str,
		params_types: ctypes.POINTER(metaffi_type_info), params_count: int,
		retvals_types: ctypes.POINTER(metaffi_type_info), retval_count: int) -> ctypes.c_void_p:
	err = ctypes.c_char_p()
	result = _xllr.load_entity(runtime_plugin_name.encode('utf-8'), module_path.encode('utf-8'),
		function_path.encode('utf-8'), params_types, params_count, retvals_types,
		retval_count, ctypes.byref(err))
	
	# check if err is not NULL
	if err.value is not None:
		msg = err.value.decode('utf-8')
		_xllr.free_string(err)  # call xllr.free_string to free the memory allocated by xllr
		raise RuntimeError(msg)
	
	return result


def free_xcall(runtime_plugin_name: str, pxcall: ctypes.c_void_p) -> None:
	err = ctypes.c_char_p()
	_xllr.free_xcall(ctypes.c_char_p(runtime_plugin_name.encode('utf-8')), pxcall, ctypes.byref(err))
	# check if err is not NULL
	if err.value is not None:
		msg = err.value.decode('utf-8')
		_xllr.free_string(err)  # call xllr.free_string to free the memory allocated by xllr
		raise RuntimeError(msg)


def make_callable(runtime_plugin_name: str, f: callable, params_types: ctypes.POINTER(metaffi_type_info), params_count: int, retvals_types: ctypes.POINTER(metaffi_type_info), retval_count: int) -> ctypes.c_void_p:
	err = ctypes.c_char_p()
	result = _xllr.make_callable(runtime_plugin_name.encode('utf-8'), f, params_types, params_count, retvals_types, retval_count, ctypes.byref(err))
	
	# check if err is not NULL
	if err.value is not None:
		msg = err.value.decode('utf-8')
		_xllr.free_string(err)  # call xllr.free_string to free the memory allocated by xllr
		raise RuntimeError(msg)
	
	return result


def alloc_cdts_buffer(params_count: int, ret_count: int) -> ctypes.c_void_p:
	return _xllr.alloc_cdts_buffer(params_count, ret_count)


def free_cdts_buffer(buffer: ctypes.c_void_p) -> None:
	_xllr.free_cdts_buffer(buffer)
