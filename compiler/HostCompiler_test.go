package main

import (
	"os"
	"strings"
	"testing"

	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
)

// --------------------------------------------------------------------
func createSimpleFunctionIDL() *IDL.IDLDefinition {
	// Create a simple function IDL
	idl := IDL.NewIDLDefinition("simple_function.json", "openjdk")

	module := IDL.NewModuleDefinition("com.example.Math")

	// Create a function
	function := IDL.NewFunctionDefinition("add")
	function.SetEntityPath("entrypoint_function", "EntryPoint_add")
	function.SetEntityPath("metaffi_guest_lib", "Math_MetaFFIGuest")
	function.SetEntityPath("module", "com.example.Math")

	// Add parameters
	param1 := &IDL.ArgDefinition{
		Name:       "a",
		Type:       IDL.INT64,
		TypeAlias:  "",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	param2 := &IDL.ArgDefinition{
		Name:       "b",
		Type:       IDL.INT64,
		TypeAlias:  "",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	function.AddParameter(param1)
	function.AddParameter(param2)

	// Add return value
	returnVal := &IDL.ArgDefinition{
		Name:       "result",
		Type:       IDL.INT64,
		TypeAlias:  "",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	function.AddReturnValues(returnVal)

	module.AddFunction(function)
	idl.AddModule(module)

	return idl
}

// --------------------------------------------------------------------
func createClassWithMethodsIDL() *IDL.IDLDefinition {
	// Create a class with methods IDL
	idl := IDL.NewIDLDefinition("class_with_methods.json", "openjdk")

	module := IDL.NewModuleDefinition("com.example.Calculator")

	// Create a class
	class := &IDL.ClassDefinition{
		Name:    "Calculator",
		Comment: "A simple calculator class",
		Tags:    make(map[string]string),
	}

	// Create constructor
	constructor := &IDL.ConstructorDefinition{
		FunctionDefinition: *IDL.NewFunctionDefinition("Calculator"),
	}
	constructor.SetEntityPath("entrypoint_function", "EntryPoint_Calculator_Calculator")
	constructor.SetEntityPath("metaffi_guest_lib", "Calculator_MetaFFIGuest")
	constructor.SetEntityPath("module", "com.example.Calculator")

	// Add return value for constructor
	returnVal := &IDL.ArgDefinition{
		Name:       "instance",
		Type:       IDL.HANDLE,
		TypeAlias:  "Calculator",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	constructor.AddReturnValues(returnVal)

	// Create method
	method := &IDL.MethodDefinition{
		FunctionDefinition: *IDL.NewFunctionDefinition("multiply"),
		InstanceRequired:   true,
	}
	method.SetEntityPath("entrypoint_function", "EntryPoint_Calculator_multiply")
	method.SetEntityPath("metaffi_guest_lib", "Calculator_MetaFFIGuest")
	method.SetEntityPath("module", "com.example.Calculator")

	// Add parameters to method
	param1 := &IDL.ArgDefinition{
		Name:       "x",
		Type:       IDL.INT64,
		TypeAlias:  "",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	param2 := &IDL.ArgDefinition{
		Name:       "y",
		Type:       IDL.INT64,
		TypeAlias:  "",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	method.AddParameter(param1)
	method.AddParameter(param2)

	// Add return value to method
	methodReturnVal := &IDL.ArgDefinition{
		Name:       "result",
		Type:       IDL.INT64,
		TypeAlias:  "",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	method.AddReturnValues(methodReturnVal)

	// Create releaser
	releaser := IDL.NewReleaserDefinition(class, "ReleaseCalculator")
	releaser.SetEntityPath("entrypoint_function", "EntryPoint_Calculator_ReleaseCalculator")
	releaser.SetEntityPath("metaffi_guest_lib", "Calculator_MetaFFIGuest")
	releaser.SetEntityPath("module", "com.example.Calculator")

	// Add parameter to releaser
	releaserParam := &IDL.ArgDefinition{
		Name:       "this_instance",
		Type:       IDL.HANDLE,
		TypeAlias:  "",
		Comment:    "",
		Tags:       make(map[string]string),
		Dimensions: 0,
	}
	releaser.AddParameter(releaserParam)

	// Add constructor, method, and releaser to class
	class.Constructors = append(class.Constructors, constructor)
	class.Methods = append(class.Methods, method)
	class.Releaser = releaser

	module.AddClass(class)
	idl.AddModule(module)

	return idl
}

// --------------------------------------------------------------------
func createGlobalsIDL() *IDL.IDLDefinition {
	// Create globals IDL
	idl := IDL.NewIDLDefinition("globals.json", "openjdk")

	module := IDL.NewModuleDefinition("com.example.Globals")

	// Create a global variable using the proper constructor
	global := IDL.NewGlobalDefinition("PI", IDL.FLOAT64, "get_PI", "")

	// Set up the getter
	getter := global.Getter
	getter.SetEntityPath("entrypoint_function", "EntryPoint_get_PI")
	getter.SetEntityPath("metaffi_guest_lib", "Globals_MetaFFIGuest")
	getter.SetEntityPath("module", "com.example.Globals")

	module.AddGlobal(global)
	idl.AddModule(module)

	return idl
}

// --------------------------------------------------------------------
func TestSimpleFunctionHost(t *testing.T) {
	def := createSimpleFunctionIDL()

	err := os.Mkdir("temp_simple_function", 0700)
	if err != nil {
		t.Fatal(err)
	}

	defer func() {
		err = os.RemoveAll("temp_simple_function")
		if err != nil {
			t.Fatal(err)
		}
	}()

	cmp := NewHostCompiler()
	err = cmp.Compile(def, "temp_simple_function", "", nil)
	if err != nil {
		t.Fatal(err)
	}

	// Verify generated file exists
	expectedFile := "temp_simple_function/simple_function_MetaFFIHost.py"
	if _, err := os.Stat(expectedFile); os.IsNotExist(err) {
		t.Errorf("Expected file %s was not generated", expectedFile)
	}
}

// --------------------------------------------------------------------
func TestClassWithMethodsHost(t *testing.T) {
	def := createClassWithMethodsIDL()

	err := os.Mkdir("temp_class_methods", 0700)
	if err != nil {
		t.Fatal(err)
	}

	defer func() {
		err = os.RemoveAll("temp_class_methods")
		if err != nil {
			t.Fatal(err)
		}
	}()

	cmp := NewHostCompiler()
	err = cmp.Compile(def, "temp_class_methods", "", nil)
	if err != nil {
		t.Fatal(err)
	}

	// Verify generated file exists
	expectedFile := "temp_class_methods/class_with_methods_MetaFFIHost.py"
	if _, err := os.Stat(expectedFile); os.IsNotExist(err) {
		t.Errorf("Expected file %s was not generated", expectedFile)
	}
}

// --------------------------------------------------------------------
func TestGlobalsHost(t *testing.T) {
	def := createGlobalsIDL()

	err := os.Mkdir("temp_globals", 0700)
	if err != nil {
		t.Fatal(err)
	}

	defer func() {
		err = os.RemoveAll("temp_globals")
		if err != nil {
			t.Fatal(err)
		}
	}()

	cmp := NewHostCompiler()
	err = cmp.Compile(def, "temp_globals", "", nil)
	if err != nil {
		t.Fatal(err)
	}

	// Verify generated file exists
	expectedFile := "temp_globals/globals_MetaFFIHost.py"
	if _, err := os.Stat(expectedFile); os.IsNotExist(err) {
		t.Errorf("Expected file %s was not generated", expectedFile)
	}
}

// --------------------------------------------------------------------
func TestTypeConversion(t *testing.T) {
	// Test type conversion functions
	tests := []struct {
		name        string
		metaffiType IDL.MetaFFIType
		isArray     bool
		expected    string
	}{
		{"int64", IDL.INT64, false, "int"},
		{"float64", IDL.FLOAT64, false, "float"},
		{"string8", IDL.STRING8, false, "str"},
		{"bool", IDL.BOOL, false, "bool"},
		{"handle", IDL.HANDLE, false, "Any"},
		{"int64_array", IDL.INT64, true, "List[int]"},
		{"string8_array", IDL.STRING8, true, "List[str]"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result := convertToPythonType(tt.metaffiType, tt.isArray)
			if result != tt.expected {
				t.Errorf("convertToPythonType(%v, %v) = %v, want %v", tt.metaffiType, tt.isArray, result, tt.expected)
			}
		})
	}
}

// --------------------------------------------------------------------
func TestMetaFFITypeForAPI(t *testing.T) {
	// Test MetaFFI type mapping for API
	tests := []struct {
		name     string
		argType  string
		expected string
	}{
		{"int64", "int64", "MetaFFITypes.metaffi_int64_type"},
		{"float64", "float64", "MetaFFITypes.metaffi_float64_type"},
		{"string8", "string8", "MetaFFITypes.metaffi_string8_type"},
		{"bool", "bool", "MetaFFITypes.metaffi_bool_type"},
		{"handle", "handle", "MetaFFITypes.metaffi_handle_type"},
		{"int64_array", "int64_array", "MetaFFITypes.metaffi_int64_array_type"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			arg := &IDL.ArgDefinition{
				Name:       "test",
				Type:       IDL.MetaFFIType(tt.argType),
				Dimensions: 0,
			}
			if strings.Contains(tt.argType, "_array") {
				arg.Dimensions = 1
			}

			result := getMetaFFITypeForAPI(arg)
			if result != tt.expected {
				t.Errorf("getMetaFFITypeForAPI(%v) = %v, want %v", tt.argType, result, tt.expected)
			}
		})
	}
}

//--------------------------------------------------------------------
