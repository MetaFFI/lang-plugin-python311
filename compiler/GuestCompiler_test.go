package main

import (
	"strings"
	"testing"

	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
)

// --------------------------------------------------------------------
func createGuestTestIDL() *IDL.IDLDefinition {
	// Create a test IDL for guest compilation
	idl := IDL.NewIDLDefinition("guest_test.json", "python3")

	module := IDL.NewModuleDefinition("test_module")

	// Create a function
	function := IDL.NewFunctionDefinition("test_function")
	function.SetEntityPath("entrypoint_function", "EntryPoint_test_function")
	function.SetEntityPath("metaffi_guest_lib", "test_MetaFFIGuest")
	function.SetEntityPath("module", "test_module")

	module.AddFunction(function)
	idl.AddModule(module)

	return idl
}

// --------------------------------------------------------------------
func TestGuestCompilerNotRequired(t *testing.T) {
	def := createGuestTestIDL()

	cmp := NewGuestCompiler()
	err := cmp.Compile(def, "temp_output", "test", nil)

	if err == nil {
		t.Fatal("Expected error for guest compilation, but got none")
	}

	errorMsg := err.Error()
	if !strings.Contains(errorMsg, "compile_to_guest not required for Python311") {
		t.Errorf("Expected error message to contain 'compile_to_guest not required for Python311', got: %s", errorMsg)
	}

	if !strings.Contains(errorMsg, "Python does not require dedicated entrypoints") {
		t.Errorf("Expected error message to contain explanation about Python not requiring entrypoints, got: %s", errorMsg)
	}
}

//--------------------------------------------------------------------
