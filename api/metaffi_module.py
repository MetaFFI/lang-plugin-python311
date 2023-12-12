import ctypes.util
from typing import *
import metaffi_types
import xllr_wrapper
import pycdts_converter
from metaffi_types import *
import metaffi_runtime



def call_sub(pxcall, pcontext, param_metaffi_types, retval_metaffi_types, *args):
	# convert args to CDTS
	
	param_metaffi_types = tuple(param_metaffi_types)
	retval_metaffi_types = tuple(retval_metaffi_types)
	
	# call xcall
	if len(param_metaffi_types) > 0 or len(retval_metaffi_types) > 0:

		cdts = pycdts_converter.py_cdts_converter.convert_host_params_to_cdts(args, param_metaffi_types, len(retval_metaffi_types))

		out_err = ctypes.c_char_p()
		out_err_len = ctypes.c_uint64()

		pxcall(pcontext, ctypes.c_void_p(cdts), ctypes.byref(out_err), ctypes.byref(out_err_len))

		if out_err_len.value > 0:
			raise RuntimeError(out_err.value[:out_err_len.value].decode('utf-8'))

		# convert return values to python
		if len(retval_metaffi_types) == 0:
			return
		
		return pycdts_converter.py_cdts_converter.convert_host_return_values_from_cdts(ctypes.c_void_p(cdts), 1)
		
	else:
		out_err = ctypes.c_char_p()
		out_err_len = ctypes.c_uint64()
		pxcall(pcontext, ctypes.byref(out_err), ctypes.byref(out_err_len))
		if out_err_len.value > 0:
			raise RuntimeError(out_err.raw[:out_err_len.value].decode('utf-8'))


XCallParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
XCallNoParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
XCallParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
XCallNoParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint64))


class MetaFFIModule:
	def __init__(self, runtime: metaffi_runtime.MetaFFIRuntime, xllr:xllr_wrapper._XllrWrapper, module_path: str):
		self.runtime = runtime
		self.xllr = xllr
		self.module_path = module_path
	
	def load(self, function_path: str, params_metaffi_types: List[metaffi_types.metaffi_type_with_alias],
			retval_metaffi_types: List[metaffi_types.metaffi_type_with_alias]) -> Callable[..., Tuple[Any, ...]]:
		
		if params_metaffi_types is None:
			params_metaffi_types = []
			
		if retval_metaffi_types is None:
			retval_metaffi_types = []
		
		# Create ctypes arrays for params_metaffi_types and retval_metaffi_types
		ParamsArray = metaffi_types.metaffi_type_with_alias * len(params_metaffi_types)
		params_array = ParamsArray(*params_metaffi_types)
		
		RetvalArray = metaffi_types.metaffi_type_with_alias * len(retval_metaffi_types)
		retval_array = RetvalArray(*retval_metaffi_types)
		
		# Call xllr.load_function
		pxcall_and_context = self.xllr.load_function('xllr.'+self.runtime.runtime_plugin, self.module_path, function_path, params_array, retval_array, len(params_metaffi_types), len(retval_metaffi_types))
		
		pxcall_and_context_array = ctypes.cast(pxcall_and_context, ctypes.POINTER(ctypes.c_void_p * 2))
		
		pxcall = None
		if len(params_metaffi_types) > 0 and len(retval_metaffi_types) > 0:
			pxcall = XCallParamsRetType(pxcall_and_context_array.contents[0])
		elif len(params_metaffi_types) > 0 and len(retval_metaffi_types) == 0:
			pxcall = XCallParamsNoRetType(pxcall_and_context_array.contents[0])
		elif len(params_metaffi_types) == 0 and len(retval_metaffi_types) > 0:
			pxcall = XCallNoParamsRetType(pxcall_and_context_array.contents[0])
		else:
			pxcall = XCallNoParamsNoRetType(pxcall_and_context_array.contents[0])
		
		
		context = pxcall_and_context_array.contents[1]
		
		param_types_without_alias = []
		for t in params_metaffi_types:
			param_types_without_alias.append(t.type)
		retval_types_without_alias = []
		for t in retval_metaffi_types:
			retval_types_without_alias.append(t.type)
		
		func_lambda: Callable[..., ...] = lambda *args: call_sub(pxcall, context, param_types_without_alias, retval_types_without_alias, *args)
		
		return func_lambda
		
		
		
