import ctypes

ReleaserFuncType = ctypes.CFUNCTYPE(None, ctypes.c_void_p)

class metaffi_handle:
	def __init__(self, h, runtime_id, releaser):
		self.handle = h
		self.runtime_id = runtime_id
		self.releaser = ctypes.cast(releaser, ReleaserFuncType)
	
	def __del__(self):
		if self.releaser:
			self.releaser(ctypes.c_void_p(self.handle))