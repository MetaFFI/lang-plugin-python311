package main

const HostHeaderTemplate = `
# Code generated by OpenFFI. DO NOT EDIT.
# Guest code for {{.IDLFilenameWithExtension}}
`

const HostImports = `
from ctypes import *
import ctypes.util
from typing import List
import platform
import os
`

const HostHelperFunctions = `
xllrHandle = None
def free_openffi():
		return # TODO: implement free library. not supported by cdll.

def load_xllr():
	global xllrHandle
	if xllrHandle == None:
		xllrHandle = cdll.LoadLibrary(get_filename_to_load('xllr'))

def get_filename_to_load(fname):
	osname = platform.system()
	if osname == 'Windows':
		return os.getenv('OPENFFI_HOME')+'\\'+ fname + '.dll'
	elif osname == 'Darwin':
		return os.getenv('OPENFFI_HOME')+'/' + fname + '.dylib'
	else:
		return os.getenv('OPENFFI_HOME')+'/' + fname + '.so' # for everything that is not windows or mac, return .so

`

const HostFunctionStubsTemplate = `
{{$pfn := .IDLFilename}}
{{range $mindex, $m := .Modules}}
{{$targetLang := $m.TargetLanguage}}
# Code to call foreign functions in module {{$m.Name}} via XLLR
{{range $findex, $f := $m.Functions}}
# Call to foreign {{$f.PathToForeignFunction.function}}
{{$f.PathToForeignFunction.function}}_id = -1
def {{$f.PathToForeignFunction.function}}({{range $index, $elem := $f.Parameters}}{{if $index}},{{end}} {{$elem.Name}}:{{ToPythonType $elem.Type}}{{end}}) -> ({{range $index, $elem := $f.ReturnValues}}{{if $index}},{{end}}{{ToPythonType $elem.Type}}{{end}}):
	
	# serialize parameters
	req = {{$f.ParametersType}}()
	{{range $index, $elem := $f.Parameters}}

	if getattr(req.{{$elem.Name}}, 'extend', None) != None: # if repeated value, use append
		req.{{$elem.Name}}.extend({{$elem.Name}})
	elif getattr(req.{{$elem.Name}}, 'CopyFrom', None) != None: # if proto message
		req.{{$elem.Name}}.CopyFrom({{$elem.Name}})
	else:
		req.{{$elem.Name}} = {{$elem.Name}}

	{{end}}

	# load XLLR
	load_xllr()

	# load function
	runtime_plugin = """xllr.{{$targetLang}}""".encode("utf-8")

	if {{$f.PathToForeignFunction.function}}_id == -1:
		function_path = """{{$f.PathToForeignFunctionAsString}}""".encode("utf-8")
        err = POINTER(c_ubyte)()
        out_err = POINTER(POINTER(c_ubyte))(c_void_p(addressof(err)))
        err_len = c_uint32()
        out_err_len = POINTER(c_uint64)(c_void_p(addressof(err_len)))

		{{$f.PathToForeignFunction.function}}_id = xllrHandle.load_function(runtime_plugin, len(runtime_plugin), function_path, len(function_path))
		if {{$f.PathToForeignFunction.function}}_id == -1: # failed to load function
			err_text = string_at(out_err.contents, out_err_len.contents.value)
			raise RuntimeError('\n'+str(err_text).replace("\\n", "\n"))

	# call function

	# in parameters
	in_params = req.SerializeToString()
	in_params_len = c_uint64(len(in_params))

	# ret
	ret = POINTER(c_ubyte)()
	out_ret = POINTER(POINTER(c_ubyte))(c_void_p(addressof(ret)))
	ret_len = c_uint64()
	out_ret_len = POINTER(c_uint64)(c_void_p(addressof(ret_len)))

	is_error = c_int8()
	out_is_error = POINTER(c_int8)(c_void_p(addressof(is_error)))
	out_is_error.contents.value = 0

	global xllrHandle
	xllrHandle.call(runtime_plugin, len(runtime_plugin), \
									{{$f.PathToForeignFunction.function}}_id, \
									in_params, in_params_len, \
									None, None, \
									out_ret, out_ret_len, \
									out_is_error)
	
	
	# deserialize result

	protoData = string_at(out_ret.contents, out_ret_len.contents.value)

	# check for error
	if out_is_error.contents.value != 0:
		raise RuntimeError('\n'+str(protoData).replace("\\n", "\n"))

	ret = {{$f.ReturnValuesType}}()
	ret.ParseFromString(protoData)

	return {{range $index, $elem := $f.ReturnValues}}{{if $index}},{{end}}ret.{{$elem.Name}}{{end}}

{{end}}{{end}}
`
