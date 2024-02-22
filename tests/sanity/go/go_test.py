import platform
import unittest
import collections
import sys
import ctypes

import metaffi
import metaffi.metaffi_runtime
import metaffi.metaffi_module
import metaffi.metaffi_types

runtime: metaffi.metaffi_runtime.MetaFFIRuntime = None
test_runtime_module: metaffi.metaffi_module.MetaFFIModule = None
test_map_module: metaffi.metaffi_module.MetaFFIModule = None


def init():
	global runtime
	global test_runtime_module
	global test_map_module
	
	runtime = metaffi.metaffi_runtime.MetaFFIRuntime('go')
	runtime.load_runtime_plugin()
	if platform.system() == 'Windows':
		test_runtime_module = runtime.load_module('TestRuntime_MetaFFIGuest.dll')
	elif platform.system() == 'Linux':
		test_runtime_module = runtime.load_module('TestRuntime_MetaFFIGuest.so')
	else:
		raise Exception(f'Unsupported system {platform.system()}')


def fini():
	# TODO
	# global runtime
	# runtime.release_runtime_plugin()
	pass


class TestSanity(unittest.TestCase):
	
	
	@classmethod
	def setUpClass(cls):
		init()
		
	@classmethod
	def tearDownClass(cls):
		fini()
	
	
	def test_hello_world(self):
		global test_runtime_module
		
		# load hello world
		hello_world = test_runtime_module.load('callable=HelloWorld', None, None)
		hello_world()
	
	def test_returns_an_error(self):
		try:
			returns_an_error = test_runtime_module.load('callable=ReturnsAnError', None, None)
			returns_an_error()
			self.fail('Test should have failed')
		except:
			pass
	
	def test_div_integers(self):
		params_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_int64_type),
						metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_int64_type)]
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_float32_type)]
		
		div_integers = test_runtime_module.load('callable=DivIntegers', params_type, ret_type)
		
		res = div_integers(1, 2)
		if res[0] != 0.5:
			self.fail('Expected 0.5, got: '+str(res[0]))
		
		try:
			div_integers(1, 0)
			self.fail('Expected an error - divisor is 0')
		except:
			pass
	
	
	def test_join_strings(self):
		params_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_array_type)]
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		
		joinStrings = test_runtime_module.load('callable=JoinStrings', params_type, ret_type)
		
		res = joinStrings(['one', 'two', 'three'])
		if res[0] != 'one,two,three':
			self.fail('Expected one,two,three, got: '+str(res[0]))
	
	
	def test_wait_a_bit(self):
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_int64_type)]
		
		getFiveSeconds = test_runtime_module.load('global=FiveSeconds,getter', None, ret_type)
		
		fiveSeconds = getFiveSeconds()
		if fiveSeconds[0] != 5000000000:
			self.fail('Expected 5000000000, got: {}'.format(fiveSeconds[0]))
		
		fiveSeconds = fiveSeconds[0]
		
		params_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_int64_type)]
		
		waitABit = test_runtime_module.load('callable=WaitABit', params_type, None)
		
		waitABit(fiveSeconds)
		
	
	def test_test_map(self):
		# load functions
		
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type)]
		newTestMap = test_runtime_module.load('callable=NewTestMap', None, ret_type)
		
		param_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type),
					metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type),
					metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_any_type)]
		testMapSet = test_runtime_module.load('callable=TestMap.Set,instance_required', param_type, None)
		
		param_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type),
					metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_any_type)]
		testMapGet = test_runtime_module.load('callable=TestMap.Get,instance_required', param_type, ret_type)
		
		param_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type),
					metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_bool_type)]
		testMapContains = test_runtime_module.load('callable=TestMap.Contains,instance_required', param_type, ret_type)
		
		param_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type),
						metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		testMapNameSetter = test_runtime_module.load('field=TestMap.Name,instance_required,setter', param_type, None)
		
		param_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type)]
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		testMapNameGetter = test_runtime_module.load('field=TestMap.Name,instance_required,getter', param_type, ret_type)
		
		map = newTestMap()
		map = map[0]
		
		testMapSet(map, 'x', 250)
		
		res = testMapContains(map, 'x')
		if not res[0]:
			self.fail('Map should contain x')
		
		res = testMapGet(map, 'x')
		if res[0] != 250:
			self.fail('x should be 250')
		
		deq = collections.deque()
		deq.append(600)
		testMapSet(map, 'z', deq)
		
		mapped_deq = testMapGet(map, 'z')
		mapped_deq = mapped_deq[0]
		val = mapped_deq.pop()
		if val != 600:
			self.fail('mapped_deq should contain 600')
		
		testMapNameSetter(map, 'MyName')
		
		name = testMapNameGetter(map)
		if name[0] != 'MyName':
			self.fail('Expected name is MyName. Received: '+name[0])
		
	def test_test_map_with_cpp_object(self):
		pass

if __name__ == '__main__':
	unittest.main()
	