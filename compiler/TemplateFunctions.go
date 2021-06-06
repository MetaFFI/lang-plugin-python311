package main

import "C"
import (
	compiler "github.com/OpenFFI/plugin-sdk/compiler/go"
	"os"
	"strings"
)

var templatesFuncMap = map[string]interface{}{
	"ConvertToCPythonType": convertToCPythonType,
	"ConvertToPythonType": convertToPythonType,
	"ConvertToPythonTypeFromField": convertToPythonTypeFromField,
	"GetEnvVar":           getEnvVar,
	"CalculateArgsLength": calculateArgsLength,
	"CalculateArgLength":  calculateArgLength,
	"Add":                 add,
}

//--------------------------------------------------------------------
func add(x int, y int) int{
	return x + y
}
//--------------------------------------------------------------------
func calculateArgLength(f *compiler.FieldDefinition) int{

	if f.IsString(){
		if f.IsArray{
			return 3 // pointer to string array, pointer to sizes array, length of array
		} else {
			return 2 // pointer to string, size of string
		}
	} else {
		if f.IsArray{
			return 2 // pointer to type array, length of array
		} else {
			return 1 // value
		}
	}
}
//--------------------------------------------------------------------
func calculateArgsLength(fields []*compiler.FieldDefinition) int{

	length := 0

	for _, f := range fields{
		length += calculateArgLength(f)
	}

	return length
}
//--------------------------------------------------------------------
func getEnvVar(env string) string{
	return os.Getenv(env)
}
//--------------------------------------------------------------------
func convertToPythonTypeFromField(definition *compiler.FieldDefinition) string{
	return convertToPythonType(definition.Type, definition.IsArray)
}
//--------------------------------------------------------------------
func convertToPythonType(openffiType string, isArray bool) string{

	openffiType = strings.ReplaceAll(openffiType, "openffi_", "")

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

		case compiler.STRING: fallthrough
		case compiler.STRING8: fallthrough
		case compiler.STRING16: fallthrough
		case compiler.STRING32:
			res = "str"

		default:
			panic("Unsupported OpenFFI Type "+openffiType)
	}

	if isArray{
		res = "List["+res+"]"
	}

	return res
}
//--------------------------------------------------------------------
func convertToCPythonType(openffiType string) string{

	openffiType = strings.ReplaceAll(openffiType, "openffi_", "")

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

		case compiler.STRING: return "c_char_p"
		case compiler.STRING8: return "c_char_p"
		case compiler.STRING16: return "c_wchar_p"
		case compiler.STRING32: return "c_wchar_p"

		default:
			panic("Unsupported OpenFFI Type "+openffiType)
	}
}
//--------------------------------------------------------------------