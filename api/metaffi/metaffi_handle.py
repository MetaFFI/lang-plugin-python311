import ctypes

ReleaserFuncType = ctypes.CFUNCTYPE(None, ctypes.c_void_p)


class metaffi_handle:
	def __init__(self, h, runtime_id, releaser):
		self.handle = h
		self.runtime_id = runtime_id
		self.releaser = ctypes.cast(releaser, ReleaserFuncType)
		
	def release(self):
		if self.releaser:
			self.releaser(ctypes.c_void_p(self.handle))
	
	def __exit__(self, exc_type, exc_val, exc_tb):
		self.release()
	
	def __del__(self):
		self.release()
		