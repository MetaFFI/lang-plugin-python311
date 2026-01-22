package main

import (
	"fmt"

	"github.com/MetaFFI/sdk/idl_entities/go/IDL"
)

// --------------------------------------------------------------------
type GuestCompiler struct {
	def            *IDL.IDLDefinition
	outputDir      string
	outputFilename string
}

// --------------------------------------------------------------------
func NewGuestCompiler() *GuestCompiler {
	return &GuestCompiler{}
}

// --------------------------------------------------------------------
func (this *GuestCompiler) Compile(definition *IDL.IDLDefinition, outputDir string, outputFilename string, guestOptions map[string]string) (err error) {

	// For Python311, guest compilation is not required
	// Python doesn't need dedicated entrypoints (no code changes needed to call from C)
	return fmt.Errorf("compile_to_guest not required for Python311 - Python does not require dedicated entrypoints to be called from C")

}

//--------------------------------------------------------------------
