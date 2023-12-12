from ctypes import *
from metaffi_types import *
import platform
import os


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
		return os.getenv(
			'METAFFI_HOME') + '/' + fname + '.so'  # for everything that is not windows or mac, return .so


class _XllrWrapper:
	def __init__(self):
		if platform.system() == 'Windows':
			os.add_dll_directory(os.environ['METAFFI_HOME'])
			os.add_dll_directory(os.environ['METAFFI_HOME'] + '\\bin')
		
		self.xllr = cdll.LoadLibrary(get_dynamic_lib_path_from_metaffi_home('xllr'))
		
		# Set argtypes and restype
		self.xllr.load_runtime_plugin.argtypes = [ctypes.c_char_p, ctypes.c_uint32, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint32)]
		self.xllr.load_runtime_plugin.restype = None
		
		self.xllr.free_runtime_plugin.argtypes = [ctypes.c_char_p, ctypes.c_uint32, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint32)]
		self.xllr.free_runtime_plugin.restype = None
		
		self.xllr.load_function.argtypes = [ctypes.c_char_p, ctypes.c_uint32, ctypes.c_char_p, ctypes.c_uint32, ctypes.c_char_p, ctypes.c_uint32, ctypes.POINTER(metaffi_type_with_alias), ctypes.POINTER(metaffi_type_with_alias), ctypes.c_uint8, ctypes.c_uint8, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint32)]
		self.xllr.load_function.restype = ctypes.POINTER(ctypes.c_void_p)
		
		self.xllr.free_function.argtypes = [ctypes.c_char_p, ctypes.c_uint32, ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint32)]
		self.xllr.free_function.restype = None
		
		self.xllr.alloc_cdts_buffer.argtypes = [ctypes.c_uint64, ctypes.c_uint64]
		self.xllr.alloc_cdts_buffer.restype = ctypes.c_void_p
	
	def load_runtime_plugin(self, runtime_plugin: str) -> None:
		err = ctypes.c_char_p()
		err_len = ctypes.c_uint32()
		self.xllr.load_runtime_plugin(runtime_plugin.encode('utf-8'), len(runtime_plugin), ctypes.byref(err), ctypes.byref(err_len))
		if err_len.value > 0:
			raise RuntimeError(err.value[:err_len.value].decode('utf-8'))
	
	def free_runtime_plugin(self, runtime_plugin: str) -> None:
		err = ctypes.c_char_p()
		err_len = ctypes.c_uint32()
		self.xllr.free_runtime_plugin(runtime_plugin.encode('utf-8'), len(runtime_plugin), ctypes.byref(err), ctypes.byref(err_len))
		if err_len.value > 0:
			raise RuntimeError(err.value[:err_len.value].decode('utf-8'))
	
	def load_function(self, runtime_plugin_name: str, module_path: str, function_path: str,
						params_types: ctypes.POINTER(metaffi_type_with_alias),
						retvals_types: ctypes.POINTER(metaffi_type_with_alias), params_count: int,
						retval_count: int) -> ctypes.c_void_p:
		err = ctypes.c_char_p()
		err_len = ctypes.c_uint32()
		result = self.xllr.load_function(runtime_plugin_name.encode('utf-8'), len(runtime_plugin_name), module_path.encode('utf-8'), len(module_path),
										function_path.encode('utf-8'), len(function_path), params_types, retvals_types, params_count,
										retval_count, ctypes.byref(err), ctypes.byref(err_len))
		if err_len.value > 0:
			raise RuntimeError(err.value[:err_len.value].decode('utf-8'))
		return result
	
	def free_function(self, runtime_plugin_name: str, pff: ctypes.POINTER(ctypes.c_void_p)) -> None:
		err = ctypes.create_string_buffer(1000)
		err_len = ctypes.c_uint32()
		self.xllr.free_function(runtime_plugin_name, len(runtime_plugin_name), pff, ctypes.byref(err), ctypes.byref(err_len))
		if err_len.value > 0:
			raise RuntimeError(err.value[:err_len.value].decode('utf-8'))
	
	def alloc_cdts_buffer(self, params_count: int, ret_count: int) -> ctypes.c_void_p:
		return self.xllr.alloc_cdts_buffer(params_count, ret_count)


xllr_wrapper: _XllrWrapper = _XllrWrapper()
