import unittest
import metaffi
import metaffi.metaffi_runtime
import metaffi.metaffi_module
import metaffi.metaffi_types

runtime: metaffi.metaffi_runtime.MetaFFIRuntime = None


def init():
	global runtime
	runtime = metaffi.metaffi_runtime.MetaFFIRuntime('openjdk')


def fini():
	global runtime
	runtime.release_runtime_plugin()


class Log4jlogger:
	def __init__(self, logger_name: str):
		global runtime
		log4j_api_module = runtime.load_module('log4j-api-2.21.1.jar;log4j-core-2.21.1.jar')
		
		# load methods
		params_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		ret_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type, 'org.apache.logging.log4j.Logger')]
		getLogger = log4j_api_module.load('class=org.apache.logging.log4j.LogManager,callable=getLogger', params_type, ret_type)
		
		params_type = [metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_handle_type),
						metaffi.metaffi_types.new_metaffi_type_info(metaffi.metaffi_types.MetaFFITypes.metaffi_string8_type)]
		self.perror = log4j_api_module.load('class=org.apache.logging.log4j.Logger,callable=error,instance_required', params_type, None)
		self.pwarning = log4j_api_module.load('class=org.apache.logging.log4j.Logger,callable=warn,instance_required', params_type, None)
		
		# call constructor
		myLoggerHandle = getLogger(logger_name)
		self.myLoggerHandle = myLoggerHandle[0]
	
	def error(self, msg: str):
		self.perror(self.myLoggerHandle, msg)
	

class TestLog4j(unittest.TestCase):
	
	@classmethod
	def setUpClass(cls):
		init()
	
	@classmethod
	def tearDownClass(cls):
		fini()
	
	def test_log(self):
		logger = Log4jlogger('My Logger!')
		logger.error('Info From Python!')
