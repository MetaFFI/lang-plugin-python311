import ctypes.util
import metaffi.xllr_wrapper
from metaffi.metaffi_types import *
import metaffi.metaffi_runtime

XCallParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
XCallNoParamsRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
XCallParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_uint64))
XCallNoParamsNoRetType = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint64))

create_lambda_python_code = ctypes.c_char_p.in_dll(metaffi.xllr_wrapper.xllr_python3, 'create_lambda_python_code').value.decode('utf-8')
exec(create_lambda_python_code)


def make_metaffi_callable(f: Callable) -> Callable:
	params_metaffi_types, retval_metaffi_types = metaffi.metaffi_types.get_callable_types(f)
	
	params_type = ctypes.c_uint64 * len(params_metaffi_types)
	params_array = params_type(*params_metaffi_types)
	
	retvals_type = ctypes.c_uint64 * len(retval_metaffi_types)
	retvals_array = retvals_type(*retval_metaffi_types)
	
	err = ctypes.c_char_p()
	err_len = ctypes.c_uint32()
	
	xllr_python3_bytes = 'xllr.python311'.encode('utf-8')
	
	pxcall_and_context_array = metaffi.xllr_wrapper.xllr.make_callable(xllr_python3_bytes, len(xllr_python3_bytes), f, params_array, retvals_array, len(params_metaffi_types), len(retval_metaffi_types), ctypes.byref(err), ctypes.byref(err_len))
	
	pxcall_and_context_array = ctypes.cast(pxcall_and_context_array, ctypes.POINTER(ctypes.c_void_p * 2))
	
	if len(params_metaffi_types) > 0 and len(retval_metaffi_types) > 0:
		pxcall = XCallParamsRetType(pxcall_and_context_array.contents[0])
	elif len(params_metaffi_types) > 0 and len(retval_metaffi_types) == 0:
		pxcall = XCallParamsNoRetType(pxcall_and_context_array.contents[0])
	elif len(params_metaffi_types) == 0 and len(retval_metaffi_types) > 0:
		pxcall = XCallNoParamsRetType(pxcall_and_context_array.contents[0])
	else:
		pxcall = XCallNoParamsNoRetType(pxcall_and_context_array.contents[0])
	
	context = pxcall_and_context_array.contents[1]
	
	res = create_lambda(pxcall, context, params_metaffi_types, retval_metaffi_types)
	setattr(res, 'pxcall_and_context', ctypes.addressof(pxcall_and_context_array.contents))
	setattr(res, 'params_metaffi_types', params_metaffi_types)
	setattr(res, 'retval_metaffi_types', retval_metaffi_types)
	return res


class MetaFFIModule:
	def __init__(self, runtime: metaffi.metaffi_runtime.MetaFFIRuntime, xllr: metaffi.xllr_wrapper._XllrWrapper, module_path: str):
		self.runtime = runtime
		self.xllr = xllr
		self.module_path = module_path
	
	def load(self, function_path: str, params_metaffi_types: Tuple[metaffi.metaffi_types.metaffi_type_info] | None,
			retval_metaffi_types: Tuple[metaffi.metaffi_types.metaffi_type_info] | None) -> Callable[..., Tuple[Any, ...]]:
		
		if params_metaffi_types is None:
			params_metaffi_types = tuple()
		
		if retval_metaffi_types is None:
			retval_metaffi_types = tuple()
		
		# Create ctypes arrays for params_metaffi_types and retval_metaffi_types
		ParamsArray = metaffi.metaffi_types.metaffi_type_info * len(params_metaffi_types)
		params_array = ParamsArray(*params_metaffi_types)
		
		RetvalArray = metaffi.metaffi_types.metaffi_type_info * len(retval_metaffi_types)
		retval_array = RetvalArray(*retval_metaffi_types)
		
		# capsule the metaffi_type_info objects to access them in call_xcall
		if not isinstance(params_metaffi_types, tuple):
			params_metaffi_types = tuple(params_metaffi_types)
		
		if not isinstance(retval_metaffi_types, tuple):
			retval_metaffi_types = tuple(retval_metaffi_types)
		
		# Call xllr.load_function
		pxcall_and_context = self.xllr.load_function('xllr.' + self.runtime.runtime_plugin, self.module_path, function_path, params_array, retval_array, len(params_metaffi_types), len(retval_metaffi_types))
		
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
		
		func_lambda: Callable[..., ...] = lambda *args: metaffi.xllr_wrapper.xllr_python3.call_xcall(pxcall, context, params_metaffi_types, retval_metaffi_types, None if not args else args)
		
		return func_lambda
