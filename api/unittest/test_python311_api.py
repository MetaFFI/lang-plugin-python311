import unittest
import metaffi
import metaffi.metaffi_runtime
import metaffi.metaffi_module
import metaffi.metaffi_types
import os

runtime: metaffi.metaffi_runtime.MetaFFIRuntime = metaffi.metaffi_runtime.MetaFFIRuntime('python311')
current_path = os.path.dirname(os.path.realpath(__file__))

