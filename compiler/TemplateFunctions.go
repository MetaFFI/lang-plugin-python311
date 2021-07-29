package main

import "C"
import (
	compiler "github.com/OpenFFI/plugin-sdk/compiler/go"
	"os"
)

var templatesFuncMap = map[string]interface{}{
	"ConvertToCPythonType": convertToCPythonType,
	"ConvertToPythonType":  convertToPythonType,
	"ConvertToPythonTypeFromField": convertToPythonTypeFromField,
	"GetEnvVar":            getEnvVar,
	"Add":                  add,
	"GetOpenFFIType":       getOpenFFIType,
}

//--------------------------------------------------------------------
func getOpenFFIType(elem *compiler.FieldDefinition) uint64{

	var val uint64
	var found bool
	if elem.Dimensions == 0 {
		val, found = compiler.TypeStringToTypeEnum[elem.Type]
	} else {
		val, found = compiler.TypeStringToTypeEnum[elem.Type+"_array"]
	}

	if !found{
		panic("Requested type is not supported: "+elem.Type)
	}

	return val
}
//--------------------------------------------------------------------
func add(x int, y int) int{
	return x + y
}
//--------------------------------------------------------------------
func getEnvVar(env string) string{
	return os.Getenv(env)
}
//--------------------------------------------------------------------
func convertToPythonTypeFromField(definition *compiler.FieldDefinition) string{
	return convertToPythonType(definition.Type, definition.IsArray())
}
//--------------------------------------------------------------------
func convertToPythonType(openffiType compiler.OpenFFIType, isArray bool) string{

	var res string

	switch openffiType{
		case compiler.FLOAT64: fallthrough
		case compiler.FLOAT32:
			res = "float"

		case compiler.INT8: fallthrough
		case compiler.INT16: fallthrough
		case compiler.INT32: fallthrough
		case compiler.INT64: fallthrough
		case compiler.UINT8: fallthrough
		case compiler.UINT16: fallthrough
		case compiler.UINT32: fallthrough
		case compiler.UINT64: fallthrough
		case compiler.SIZE:
			res = "int"

		case compiler.BOOL:
			res = "bool"

		case compiler.STRING8: fallthrough
		case compiler.STRING16: fallthrough
		case compiler.STRING32:
			res = "str"

		case compiler.HANDLE:
			res = "py_object"

		case compiler.ANY:
			res = "Any"

		default:
			panic("Unsupported OpenFFI Type "+openffiType)
	}

	if isArray{
		res = "List["+res+"]"
	}

	return res
}
//--------------------------------------------------------------------
func convertToCPythonType(openffiType compiler.OpenFFIType) string{


	switch openffiType{
		case compiler.FLOAT64: return "c_double"
		case compiler.FLOAT32: return "c_float"
		case compiler.INT8: return "c_byte"
		case compiler.INT16: return "c_short"
		case compiler.INT32: return "c_int"
		case compiler.INT64: return "c_longlong"
		case compiler.UINT8: return "c_ubyte"
		case compiler.UINT16: return "c_ushort"
		case compiler.UINT32: return "c_uint"
		case compiler.UINT64: return "c_ulonglong"
		case compiler.SIZE: return "c_ulonglong"
		case compiler.BOOL: return "c_ubyte"

		case compiler.STRING8: return "c_char_p"
		case compiler.STRING16: return "c_wchar_p"
		case compiler.STRING32: return "c_wchar_p"

		default:
			panic("Unsupported OpenFFI Type "+openffiType)
	}
}
//--------------------------------------------------------------------