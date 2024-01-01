import ctypes
from ctypes import py_object
from typing import List
import os
import platform
import metaffi.xllr_wrapper

class _PyCdtsConverter:
	def __init__(self):
		if platform.system() == 'Windows':
			os.add_dll_directory(os.environ['METAFFI_HOME'])
			os.add_dll_directory(os.environ['METAFFI_HOME'] + '\\bin')
		
		self.xllr_python3 = ctypes.cdll.LoadLibrary(xllr_wrapper.get_dynamic_lib_path_from_metaffi_home('xllr.python3'))
		
		# Set argtypes and restype for convert_host_params_to_cdts
		self.xllr_python3.convert_host_params_to_cdts.argtypes = [py_object, py_object, ctypes.c_uint64]
		self.xllr_python3.convert_host_params_to_cdts.restype = ctypes.c_void_p
		
		# Set argtypes and restype for convert_host_return_values_from_cdts
		self.xllr_python3.convert_host_return_values_from_cdts.argtypes = [ctypes.c_void_p, ctypes.c_uint64]
		self.xllr_python3.convert_host_return_values_from_cdts.restype = py_object
	
	def convert_host_params_to_cdts(self, params_names: py_object, params_types: py_object,
														return_values_size: int) -> ctypes.c_void_p:
		res = self.xllr_python3.convert_host_params_to_cdts(params_names, params_types, return_values_size)
		return res
	
	def convert_host_return_values_from_cdts(self, pcdts: ctypes.c_void_p, index: int) -> py_object:
		res = self.xllr_python3.convert_host_return_values_from_cdts(pcdts, index)
		return res


py_cdts_converter: _PyCdtsConverter = _PyCdtsConverter()
