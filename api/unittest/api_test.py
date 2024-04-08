import unittest
import metaffi
import metaffi.metaffi_runtime
import metaffi.metaffi_module
import metaffi.metaffi_types
import os

runtime: metaffi.metaffi_runtime.MetaFFIRuntime = metaffi.metaffi_runtime.MetaFFIRuntime('python311')
current_path = os.path.dirname(os.path.realpath(__file__))


class RuntimeTargetTest(unittest.TestCase):
	test_runtime_module: metaffi.metaffi_module.MetaFFIModule = None
	
	@classmethod
	def setUpClass(cls):
		RuntimeTargetTest.test_runtime_module = runtime.load_module(f'{current_path}/../runtime/test/runtime_test_target.py')
		
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
	
	self.skipTest('Callback from python is not implemented yet')
	
	def add(x: int, y: int) -> int:
		print('in python add from java - {}+{}'.format(x, y), file=sys.stderr)
		return x + y
	
	# make "add_callback" metaffi callable
	metaffi_callable = metaffi.metaffi_module.make_metaffi_callable(add)
	
	# load call_callback_add from Java
	param_type = [metaffi.metaffi_types.metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_callable_type)]
	testMapNameGetter = test_runtime_module.load('class=sanity.TestRuntime,callable=callCallback', param_type, None)
	
	# call call_callback_add passing add_callback
	testMapNameGetter(metaffi_callable)


if __name__ == '__main__':
	unittest.main()