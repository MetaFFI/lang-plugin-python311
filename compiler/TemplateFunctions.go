package main

import "C"
import (
	"fmt"
	"os"
	"strings"

	"github.com/MetaFFI/sdk/idl_entities/go/IDL"
)

var templatesFuncMap = map[string]interface{}{
	"ConvertToCPythonType":         convertToCPythonType,
	"ConvertToPythonType":          convertToPythonType,
	"ConvertToPythonTypeFromField": convertToPythonTypeFromField,
	"GetEnvVar":                    getEnvVar,
	"Add":                          add,
	"GetMetaFFIType":               getMetaFFIType,
	"GetMetaFFITypeForAPI":         getMetaFFITypeForAPI,
	"GenerateCodeAllocateCDTS":     generateCodeAllocateCDTS,
	"GenerateCodeXCall":            generateCodeXCall,
	"GenerateCodeReturnValues":     generateCodeReturnValues,
	"GenerateCodeGlobals":          generateCodeGlobals,
	"GenerateCodeReturn":           generateReturn,
	"GetCFuncType":                 getCFuncType,
	"GenerateCEntryPoint":          generateCEntryPoint,
	"GenerateMethodSignature":      generateMethodSignature,
	"GetMetaFFITypeFromPyType":     getMetaFFITypeFromPyType,
	"IsArray":                      isArray,
}

// --------------------------------------------------------------------
func generateMethodSignature(meth *IDL.MethodDefinition) string {
	//{{if $f.InstanceRequired}}self{{end}}{{range $index, $elem := $f.Parameters}}{{if gt $index 0}}{{if $index}},{{end}} {{$elem.Name}}:{{ConvertToPythonTypeFromField $elem}}{{end}}{{end}}->{{ConvertToPythonTypeFromField $restype}}
	params := make([]string, 0)
	retvals := make([]string, 0)

	for i, p := range meth.Parameters {
		if i == 0 && meth.InstanceRequired {
			params = append(params, "self")
		} else {
			params = append(params, fmt.Sprintf("%v:%v", p.Name, convertToPythonTypeFromField(p)))
		}
	}

	for _, r := range meth.ReturnValues {
		retvals = append(retvals, convertToPythonTypeFromField(r))
	}

	paramsString := strings.Join(params, ",")

	retvalString := ""
	if len(retvals) == 0 {
		retvalString = "None"
	} else if len(retvals) == 1 {
		retvalString = retvals[0]
	} else {
		retvalString = "(" + strings.Join(retvals, ",") + ")"
	}

	return fmt.Sprintf("(%s) -> %s", paramsString, retvalString)
}

// --------------------------------------------------------------------
func generateCEntryPoint(name string, params []*IDL.ArgDefinition, retvals []*IDL.ArgDefinition, isMethod bool, indent int) string {
	indentStr := strings.Repeat("\t", indent)

	code := fmt.Sprintf("%sdef EntryPoint_%s(", indentStr, name)

	paramNames := make([]string, 0)
	for i, p := range params {
		if i == 0 && isMethod {
			paramNames = append(paramNames, "self")
		} else {
			paramNames = append(paramNames, p.Name)
		}
	}

	code += strings.Join(paramNames, ",")
	code += "):\n"

	code += fmt.Sprintf("%s\ttry:\n", indentStr)

	// call function
	retvalNames := make([]string, 0)
	if len(retvals) > 0 {
		for _, r := range retvals {
			retvalNames = append(retvalNames, r.Name)
		}
		code += fmt.Sprintf("%s\t\t%s = foreign_function(%s)\n", indentStr, strings.Join(retvalNames, ","), strings.Join(paramNames, ","))
	} else {
		code += fmt.Sprintf("%s\t\tforeign_function(%s)\n", indentStr, strings.Join(paramNames, ","))
	}

	// return values
	if len(retvals) > 0 {
		retvalTypes := make([]string, 0)
		for _, r := range retvals {
			retvalTypes = append(retvalTypes, getMetaFFIType(r, false))
		}
		code += fmt.Sprintf("%s\t\tret_val_types = (%s)\n", indentStr, strings.Join(retvalTypes, ","))
		code += fmt.Sprintf("%s\t\treturn (None, ret_val_types, %s)\n", indentStr, strings.Join(retvalNames, ","))
	} else {
		code += fmt.Sprintf("%s\t\treturn (None,)\n", indentStr)
	}

	code += fmt.Sprintf("%s\texcept Exception as e:\n", indentStr)
	code += fmt.Sprintf("%s\t\terrdata = traceback.format_exception(*sys.exc_info())\n", indentStr)
	code += fmt.Sprintf("%s\t\treturn ('\\n'.join(errdata),)\n", indentStr)

	return code
}

// --------------------------------------------------------------------
func getCFuncType(params []*IDL.ArgDefinition, retvals []*IDL.ArgDefinition) string {
	if len(params) > 0 && len(retvals) > 0 {
		return "cfunctype_params_ret"
	} else if len(params) > 0 && len(retvals) == 0 {
		return "cfunctype_params_no_ret"
	} else if len(params) == 0 && len(retvals) > 0 {
		return "cfunctype_no_params_ret"
	} else {
		return "cfunctype_no_params_no_ret"
	}
}

// --------------------------------------------------------------------
func generateCodeGlobals(name string, indent int) string {
	indentStr := strings.Repeat("\t", indent)
	return fmt.Sprintf("%sglobal %s_id\n", indentStr, name)
}

// --------------------------------------------------------------------
func generateReturn(retvals []*IDL.ArgDefinition) string {
	if len(retvals) == 0 {
		return "return"
	} else if len(retvals) == 1 {
		return fmt.Sprintf("return %s", retvals[0].Name)
	} else {
		retvalNames := make([]string, 0)
		for _, r := range retvals {
			retvalNames = append(retvalNames, r.Name)
		}
		return fmt.Sprintf("return (%s)", strings.Join(retvalNames, ","))
	}
}

// --------------------------------------------------------------------
func generateCodeReturnValues(parameters []*IDL.ArgDefinition, retvals []*IDL.ArgDefinition) string {
	if len(retvals) == 0 {
		return ""
	}

	code := "\t# unpack results\n"
	code += "\tret_vals = python_plugin_handle.convert_host_return_values_from_cdts(xcall_return, "
	code += fmt.Sprintf("%d", len(retvals))
	code += ")\n"

	if len(retvals) == 1 {
		code += fmt.Sprintf("\t%s = ret_vals\n", retvals[0].Name)
	} else {
		for i, r := range retvals {
			code += fmt.Sprintf("\t%s = ret_vals[%d]\n", r.Name, i)
		}
	}

	return code
}

// --------------------------------------------------------------------
func generateCodeXCall(className string, funcName string, overloadIndexString string, params []*IDL.ArgDefinition, retvals []*IDL.ArgDefinition, indent int) string {
	indentStr := strings.Repeat("\t", indent)

	var funcId string
	if className == "" {
		funcId = fmt.Sprintf("%s%s_id", funcName, overloadIndexString)
	} else {
		funcId = fmt.Sprintf("%s_%s%s_id", className, funcName, overloadIndexString)
	}

	code := fmt.Sprintf("%s# xcall function\n", indentStr)

	if len(params) > 0 && len(retvals) > 0 {
		code += fmt.Sprintf("%sxcall_return = xllr_handle.xcall_params_ret(%s, xcall_params, %s)\n", indentStr, funcId, funcId)
	} else if len(params) > 0 && len(retvals) == 0 {
		code += fmt.Sprintf("%sxllr_handle.xcall_params_no_ret(%s, xcall_params)\n", indentStr, funcId)
	} else if len(params) == 0 && len(retvals) > 0 {
		code += fmt.Sprintf("%sxcall_return = xllr_handle.xcall_no_params_ret(%s)\n", indentStr, funcId)
	} else {
		code += fmt.Sprintf("%sxllr_handle.xcall_no_params_no_ret(%s)\n", indentStr, funcId)
	}

	return code
}

// --------------------------------------------------------------------
func generateCodeAllocateCDTS(params []*IDL.ArgDefinition, retvals []*IDL.ArgDefinition, isObjectMember bool) string {
	if len(params) == 0 && len(retvals) == 0 {
		return ""
	}

	code := "\t# allocate CDTS buffer\n"

	if len(params) > 0 {
		code += "\tparams = ("
		paramNames := make([]string, 0)
		paramTypes := make([]string, 0)

		for i, p := range params {
			if isObjectMember && i == 0 {
				paramNames = append(paramNames, "self.obj_handle")
			} else {
				name := strings.Replace(p.Name, "*", "", -1) // remove "*" or "**" from the params
				paramNames = append(paramNames, fmt.Sprintf("%v", name))
			}

			paramTypes = append(paramTypes, getMetaFFIType(p, false))
		}

		code += fmt.Sprintf("%s)\n", strings.Join(paramNames, ","))
		code += fmt.Sprintf("\tparams_types = (%s)\n", strings.Join(paramTypes, ","))
		code += "\txcall_params = python_plugin_handle.convert_host_params_to_cdts(py_object(params), py_object(params_types), 0)\n"
	} else if len(retvals) > 0 {
		code += fmt.Sprintf("\txcall_params = xllr_handle.alloc_cdts_buffer(0, %d)\n", len(retvals))
	} else {
		return ""
	}

	return code
}

// --------------------------------------------------------------------
func getMetaFFIType(elem *IDL.ArgDefinition, isObjectField bool) string {

	var handle uint64
	var found bool

	if elem.Type == IDL.ANY {
		if isObjectField {
			return fmt.Sprintf("dynamicTypeToMetaFFIType(obj[0].%v)", elem.Name)
		} else {
			return fmt.Sprintf("dynamicTypeToMetaFFIType(%v)", elem.Name)
		}
	}

	if elem.Dimensions == 0 && strings.HasSuffix(string(elem.Type), "_array") {
		panic(fmt.Sprintf("Argument %v type is %v, although its dimensions are larger than 0", elem.Name, elem.Type))
	}

	if elem.Dimensions > 0 && !strings.HasSuffix(string(elem.Type), "_array") {
		elem.Type = IDL.MetaFFIType(string(elem.Type) + "_array")
	}

	handle, found = IDL.TypeStringToTypeEnum[elem.Type]

	if !found {
		panic(fmt.Sprintf("Requested type is not supported: %v. Name: %v", elem.Type, elem.Name))
	}

	return fmt.Sprintf("%v", handle)
}

// --------------------------------------------------------------------
// New function for MetaFFI API approach
func getMetaFFITypeForAPI(elem *IDL.ArgDefinition) string {
	if elem.Dimensions > 0 {
		// Handle array types
		baseType := strings.ReplaceAll(string(elem.Type), "_array", "")
		return fmt.Sprintf("MetaFFITypes.metaffi_%s_array_type", baseType)
	}

	switch elem.Type {
	case "int64":
		return "MetaFFITypes.metaffi_int64_type"
	case "int32":
		return "MetaFFITypes.metaffi_int32_type"
	case "int16":
		return "MetaFFITypes.metaffi_int16_type"
	case "int8":
		return "MetaFFITypes.metaffi_int8_type"
	case "uint64":
		return "MetaFFITypes.metaffi_uint64_type"
	case "uint32":
		return "MetaFFITypes.metaffi_uint32_type"
	case "uint16":
		return "MetaFFITypes.metaffi_uint16_type"
	case "uint8":
		return "MetaFFITypes.metaffi_uint8_type"
	case "float64":
		return "MetaFFITypes.metaffi_float64_type"
	case "float32":
		return "MetaFFITypes.metaffi_float32_type"
	case "string8":
		return "MetaFFITypes.metaffi_string8_type"
	case "string16":
		return "MetaFFITypes.metaffi_string16_type"
	case "string32":
		return "MetaFFITypes.metaffi_string32_type"
	case "bool":
		return "MetaFFITypes.metaffi_bool_type"
	case "handle":
		return "MetaFFITypes.metaffi_handle_type"
	case "any":
		return "MetaFFITypes.metaffi_any_type"
	default:
		// Try to handle type aliases
		if elem.TypeAlias != "" {
			return "MetaFFITypes.metaffi_handle_type" // Custom types are typically handles
		}
		return "MetaFFITypes.metaffi_any_type"
	}
}

// --------------------------------------------------------------------
func getMetaFFITypeFromPyType(pytype string) uint64 {

	switch pytype {
	case "str":
		return IDL.TypeStringToTypeEnum[IDL.STRING8]
	case "int":
		return IDL.TypeStringToTypeEnum[IDL.INT64]
	case "float":
		return IDL.TypeStringToTypeEnum[IDL.FLOAT64]
	case "bool":
		return IDL.TypeStringToTypeEnum[IDL.BOOL]
	case "handle":
		return IDL.TypeStringToTypeEnum[IDL.HANDLE]
	default:
		panic("Unexpected PyType: " + pytype)
	}
}

// --------------------------------------------------------------------
func add(x int, y int) int {
	return x + y
}

// --------------------------------------------------------------------
func getEnvVar(env string) string {
	return os.Getenv(env)
}

// --------------------------------------------------------------------
func convertToPythonTypeFromField(definition *IDL.ArgDefinition) string {

	if strings.Contains(string(definition.Type), "_array") {

		if !definition.IsArray() {
			panic(fmt.Sprintf("Argument %v of type %v states an array, but dimensions is %v", definition.Name, definition.Type, definition.Dimensions))
		}

		typeName := strings.ReplaceAll(string(definition.Type), "_array", "")
		return convertToPythonType(IDL.MetaFFIType(typeName), definition.IsArray())
	} else {
		return convertToPythonType(definition.Type, definition.IsArray())
	}
}

// --------------------------------------------------------------------
func isArray(dimensions int) bool {
	return dimensions > 0
}

// --------------------------------------------------------------------
func convertToPythonType(metaffiType IDL.MetaFFIType, isArray bool) string {

	var res string

	switch metaffiType {
	case IDL.FLOAT64:
		fallthrough
	case IDL.FLOAT32:
		res = "float"
	case IDL.FLOAT64_ARRAY:
		fallthrough
	case IDL.FLOAT32_ARRAY:
		res = "List[float]"

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
	case IDL.INT8_ARRAY:
		fallthrough
	case IDL.INT16_ARRAY:
		fallthrough
	case IDL.INT32_ARRAY:
		fallthrough
	case IDL.INT64_ARRAY:
		fallthrough
	case IDL.UINT8_ARRAY:
		fallthrough
	case IDL.UINT16_ARRAY:
		fallthrough
	case IDL.UINT32_ARRAY:
		fallthrough
	case IDL.UINT64_ARRAY:
		fallthrough
	case IDL.SIZE_ARRAY:
		res = "List[int]"

	case IDL.BOOL:
		res = "bool"
	case IDL.BOOL_ARRAY:
		res = "List[bool]"

	case IDL.STRING8:
		fallthrough
	case IDL.STRING16:
		fallthrough
	case IDL.STRING32:
		res = "str"
	case IDL.STRING8_ARRAY:
		fallthrough
	case IDL.STRING16_ARRAY:
		fallthrough
	case IDL.STRING32_ARRAY:
		res = "List[str]"

	case IDL.CHAR8_ARRAY:
		fallthrough
	case IDL.CHAR16_ARRAY:
		fallthrough
	case IDL.CHAR32_ARRAY:
		res = "List[str]"

	case IDL.HANDLE:
		res = "Any"
	case IDL.HANDLE_ARRAY:
		res = "List[Any]"

	case IDL.ANY:
		res = "Any"
	case IDL.ANY_ARRAY:
		res = "List[Any]"

	default:
		panic("Unsupported MetaFFI Type " + metaffiType)
	}

	if isArray {
		res = "List[" + res + "]"
	}

	return res
}

// --------------------------------------------------------------------
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
