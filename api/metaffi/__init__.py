"""Python MetaFFI API"""

__version__ = "0.0.42"

__all__ = ['metaffi', 'metaffi_types', 'metaffi_runtime', 'metaffi_module', 'MetaFFIHandle', 'metaffi_types', 'xllr_wrapper', 'pycdts_converter', 'metaffi_type_info', 'MetaFFITypes', 'MetaFFIEntity']

import metaffi
from . import metaffi_types
from . import metaffi_runtime
from . import metaffi_module
from .metaffihandle import MetaFFIHandle
from . import xllr_wrapper
from . import pycdts_converter
from .metaffi_types import metaffi_type_info
from .metaffi_types import MetaFFITypes
from .metaffi_module import MetaFFIEntity


if 'create_lambda' not in globals():
	import platform
	import os
	import ctypes

	# create_lambda is a function that creates a lambda function that calls xllr.call_xcall
	def get_dynamic_lib_path_from_metaffi_home(fname: str):
		if fname != 'xllr':
			fname = f'/{fname}/xllr.{fname}'

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
		os.add_dll_directory(os.getenv('METAFFI_HOME'))
		os.add_dll_directory(os.getenv('METAFFI_HOME') + '\\python311\\')

	xllr_python3 = ctypes.cdll.LoadLibrary(get_dynamic_lib_path_from_metaffi_home('python311'))
	xllr_python3.call_xcall.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.py_object, ctypes.py_object, ctypes.py_object]
	xllr_python3.call_xcall.restype = ctypes.py_object

	XCallParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallNoParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
	XCallNoParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint64))

	# TODO: replace pxcall and context to a single parameter
	# 		to xcall*. The current problem is that I can't find how to pass
	#		xcall* into the Tuple when calling the function from C++
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