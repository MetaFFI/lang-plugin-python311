package main

import (
	"fmt"
	. "github.com/MetaFFI/lang-plugin-python3/idl/py_extractor"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
)

// --------------------------------------------------------------------
func GenerateFunctionDefinition(name string, comment string, params []Parameter_info, retvals []string, metaffiGuestLib string) (*IDL.FunctionDefinition, error) {

	pyfunc := IDL.NewFunctionDefinition(name)
	pyfunc.Comment = comment

	for _, p := range params {
		name, err := p.GetName()
		if err != nil {
			return nil, err
		}

		pyty, err := p.GetType()
		if err != nil {
			return nil, err
		}

		isDefaultVal, err := p.GetIsDefaultValue()
		if err != nil {
			return nil, err
		}

		kind, err := p.GetKind()
		if err != nil {
            return nil, err
        }

		mffiType := pyTypeToMFFI(pyty)
		var talias string
		if mffiType == IDL.HANDLE || mffiType == IDL.HANDLE_ARRAY {
			talias = pyty
		}

		dims := 0
		if pyty == "list" {
			dims = 1
		}

		mffiparam := IDL.NewArgArrayDefinitionWithAlias(name, mffiType, dims, talias)
		mffiparam.IsOptional = isDefaultVal
		mffiparam.SetTag("kind", kind)

		pyfunc.AddParameter(mffiparam)
	}

	for i, pyty := range retvals {
		name := fmt.Sprintf("ret_%d", i)

		mffiType := pyTypeToMFFI(pyty)
		var talias string
		if mffiType == IDL.HANDLE || mffiType == IDL.HANDLE_ARRAY {
			talias = pyty
		}

		dims := 0
		if pyty == "list" {
			dims = 1
		}

		pyfunc.AddReturnValues(IDL.NewArgArrayDefinitionWithAlias(name, mffiType, dims, talias))
	}

	pyfunc.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
	pyfunc.SetFunctionPath("entrypoint_function", "EntryPoint_"+pyfunc.Name)

	return pyfunc, nil
}

// --------------------------------------------------------------------
func ExtractFunctions(pyinfo *Py_info, metaffiGuestLib string) ([]*IDL.FunctionDefinition, error) {

	functions := make([]*IDL.FunctionDefinition, 0)

	funcs, err := pyinfo.GetFunctions()
	if err != nil {
		return nil, err
	}

	for _, f := range funcs {

		params, err := f.GetParameters()
		if err != nil {
			return nil, err
		}

		name, err := f.GetName()
		if err != nil {
			return nil, err
		}

		comment, err := f.GetComment()
		if err != nil {
			return nil, err
		}

		retvals, err := f.GetReturnValues()
		if err != nil {
			return nil, err
		}

		fdef, err := GenerateFunctionDefinition(name, comment, params, retvals, metaffiGuestLib)
		if err != nil {
			return nil, err
		}

		functions = append(functions, fdef)
	}

	return functions, nil
}

//--------------------------------------------------------------------
