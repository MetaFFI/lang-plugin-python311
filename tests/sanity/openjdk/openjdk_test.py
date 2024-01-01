import unittest
import collections
import sys
import ctypes

import metaffi

runtime: metaffi.metaffi_runtime.MetaFFIRuntime = None
test_runtime_module: api.metaffi_module.MetaFFIModule = None
test_map_module: api.metaffi_module.MetaFFIModule = None


def add_callback(x, y):
	return x + y


def init():
	global runtime
	global test_runtime_module
	global test_map_module
	
	runtime = api.metaffi_runtime.MetaFFIRuntime('openjdk')
	runtime.load_runtime_plugin()
	test_runtime_module = runtime.load_module('sanity/TestRuntime.class')
	test_map_module = runtime.load_module('sanity/TestMap.class')


def fini():
	global runtime
	runtime.release_runtime_plugin()


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
		hello_world = test_runtime_module.load('class=sanity.TestRuntime,callable=helloWorld', None, None)
		hello_world()
	
	def test_returns_an_error(self):
		try:
			returns_an_error = test_runtime_module.load('class=sanity.TestRuntime,callable=returnsAnError', None, None)
			returns_an_error()
			self.fail('Test should have failed')
		except:
			pass
	
	def test_div_integers(self):
		params_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_int32_type),
		               api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_int32_type)]
		ret_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_float32_type)]
		
		div_integers = test_runtime_module.load('class=sanity.TestRuntime,callable=divIntegers', params_type, ret_type)
		
		res = div_integers(1, 2)
		if res[0] != 0.5:
			self.fail('Expected 0.5, got: ' + str(res[0]))
		
		try:
			div_integers(1, 0)
			self.fail('Expected an error - divisor is 0')
		except:
			pass
	
	def test_join_strings(self):
		params_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_string8_array_type)]
		ret_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		
		joinStrings = test_runtime_module.load('class=sanity.TestRuntime,callable=joinStrings', params_type, ret_type)
		
		res = joinStrings(['one', 'two', 'three'])
		if res[0] != 'one,two,three':
			self.fail('Expected one,two,three, got: ' + str(res[0]))
	
	def test_wait_a_bit(self):
		ret_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_int32_type)]
		
		getFiveSeconds = test_runtime_module.load('class=sanity.TestRuntime,field=fiveSeconds,getter', None, ret_type)
		
		fiveSeconds = getFiveSeconds()
		if fiveSeconds[0] != 5:
			self.fail('Expected 5, got: ' + fiveSeconds[0])
		
		fiveSeconds = fiveSeconds[0]
		
		params_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_int32_type)]
		
		waitABit = test_runtime_module.load('class=sanity.TestRuntime,callable=waitABit', params_type, None)
		
		waitABit(fiveSeconds)
	
	def test_test_map(self):
		# load functions
		
		ret_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_handle_type)]
		newTestMap = test_runtime_module.load('class=sanity.TestMap,callable=<init>', None, ret_type)
		
		param_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_handle_type),
		              api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_string8_type),
		              api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_any_type)]
		testMapSet = test_runtime_module.load('class=sanity.TestMap,callable=set,instance_required', param_type, None)
		
		param_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_handle_type),
		              api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		ret_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_any_type)]
		testMapGet = test_runtime_module.load('class=sanity.TestMap,callable=get,instance_required', param_type, ret_type)
		
		param_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_handle_type),
		              api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		ret_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_bool_type)]
		testMapContains = test_runtime_module.load('class=sanity.TestMap,callable=contains,instance_required', param_type, ret_type)
		
		param_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_handle_type),
		              api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		testMapNameSetter = test_runtime_module.load('class=sanity.TestMap,field=name,instance_required,setter', param_type, None)
		
		param_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_handle_type)]
		ret_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		testMapNameGetter = test_runtime_module.load('class=sanity.TestMap,field=name,instance_required,getter', param_type, ret_type)
		
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
			self.fail('Expected name is MyName. Received: ' + name[0])
	
	
	
	def test_callback(self):
		
		def add(x: int, y: int) -> int:
			print('in python add from java - {}+{}'.format(x, y), file=sys.stderr)
			return x+y
		
		# make "add_callback" metaffi callable
		metaffi_callable = api.metaffi_module.make_metaffi_callable(add)
		
		# load call_callback_add from Java
		param_type = [api.metaffi_types.new_metaffi_type_with_alias(api.metaffi_types.MetaFFITypes.metaffi_callable_type)]
		testMapNameGetter = test_runtime_module.load('class=sanity.TestRuntime,callable=callCallback', param_type, None)
		
		# call call_callback_add passing add_callback
		testMapNameGetter(metaffi_callable)
		


if __name__ == '__main__':
	unittest.main()
