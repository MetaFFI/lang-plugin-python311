from metaffi.xllr_wrapper import *

class MetaFFIRuntime:
	def __init__(self, runtime_plugin: str):
		self.runtime_plugin = runtime_plugin
		
	def load_runtime_plugin(self):
		xllr_wrapper.load_runtime_plugin('xllr.' + self.runtime_plugin)
	
	def release_runtime_plugin(self):
		xllr_wrapper.free_runtime_plugin('xllr.' + self.runtime_plugin)
	
	def load_module(self, module_path: str):
		import metaffi_module
		return metaffi_module.MetaFFIModule(self, xllr_wrapper, module_path)
