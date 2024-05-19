import ctypes

ReleaserFuncType = ctypes.CFUNCTYPE(None, ctypes.c_void_p)


class metaffi_handle:
	def __init__(self, h: ctypes.c_void_p, runtime_id: ctypes.c_uint64, releaser: ctypes.c_void_p):
		self.handle = ctypes.c_void_p(h)
		self.runtime_id = runtime_id
		self.releaser = ctypes.cast(releaser, ReleaserFuncType)
		
	def __del__(self):
		if self.releaser:
			self.releaser(self.handle)
		
	