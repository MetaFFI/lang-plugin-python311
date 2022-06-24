package main

import "C"
import (
	"fmt"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
	"os"
	"strings"
)

var templatesFuncMap = map[string]interface{}{
	"ConvertToCPythonType":         convertToCPythonType,
	"ConvertToPythonType":          convertToPythonType,
	"ConvertToPythonTypeFromField": convertToPythonTypeFromField,
	"GetEnvVar":                    getEnvVar,
	"Add":                          add,
	"GetMetaFFIType":               getMetaFFIType,
}

//--------------------------------------------------------------------
func getMetaFFIType(elem *IDL.ArgDefinition) uint64 {
	
	var val uint64
	var found bool
	
	if elem.Dimensions == 0 && strings.HasSuffix(string(elem.Type), "_array") {
		panic(fmt.Sprintf("Argument %v type is %v, although its dimensions are larger than 0", elem.Name, elem.Type))
	}
	
	if elem.Dimensions > 0 && !strings.HasSuffix(string(elem.Type), "_array") {
		elem.Type = IDL.MetaFFIType(string(elem.Type) + "_array")
	}
	
	val, found = IDL.TypeStringToTypeEnum[elem.Type]
	
	if !found {
		panic("Requested type is not supported: " + elem.Type)
	}
	
	return val
}

//--------------------------------------------------------------------
func add(x int, y int) int {
	return x + y
}

//--------------------------------------------------------------------
func getEnvVar(env string) string {
	return os.Getenv(env)
}

//--------------------------------------------------------------------
func convertToPythonTypeFromField(definition *IDL.ArgDefinition) string {
	return convertToPythonType(definition.Type, definition.IsArray())
}

//--------------------------------------------------------------------
func convertToPythonType(metaffiType IDL.MetaFFIType, isArray bool) string {
	
	var res string
	
	switch metaffiType {
	case IDL.FLOAT64:
		fallthrough
	case IDL.FLOAT32:
		res = "float"
	
	case IDL.INT8:
		fallthrough
	case IDL.INT16:
		fallthrough
	case IDL.INT32:
		fallthrough
	case IDL.INT64:
		fallthrough
	case IDL.UINT8:
		fallthrough
	case IDL.UINT16:
		fallthrough
	case IDL.UINT32:
		fallthrough
	case IDL.UINT64:
		fallthrough
	case IDL.SIZE:
		res = "int"
	
	case IDL.BOOL:
		res = "bool"
	
	case IDL.STRING8:
		fallthrough
	case IDL.STRING16:
		fallthrough
	case IDL.STRING32:
		res = "str"
	
	case IDL.HANDLE:
		res = "py_object"
	
	case IDL.ANY:
		res = "Any"
	
	default:
		panic("Unsupported MetaFFI Type " + metaffiType)
	}
	
	if isArray {
		res = "List[" + res + "]"
	}
	
	return res
}

//--------------------------------------------------------------------
func convertToCPythonType(metaffiType IDL.MetaFFIType) string {
	
	switch metaffiType {
	case IDL.FLOAT64:
		return "c_double"
	case IDL.FLOAT32:
		return "c_float"
	case IDL.INT8:
		return "c_byte"
	case IDL.INT16:
		return "c_short"
	case IDL.INT32:
		return "c_int"
	case IDL.INT64:
		return "c_longlong"
	case IDL.UINT8:
		return "c_ubyte"
	case IDL.UINT16:
		return "c_ushort"
	case IDL.UINT32:
		return "c_uint"
	case IDL.UINT64:
		return "c_ulonglong"
	case IDL.SIZE:
		return "c_ulonglong"
	case IDL.BOOL:
		return "c_ubyte"
	
	case IDL.STRING8:
		return "c_char_p"
	case IDL.STRING16:
		return "c_wchar_p"
	case IDL.STRING32:
		return "c_wchar_p"
	
	default:
		panic("Unsupported MetaFFI Type " + metaffiType)
	}
}

//--------------------------------------------------------------------
