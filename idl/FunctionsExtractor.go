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

	isAdd_dict_named_args := false
	isAllKeywordOnlyOptional := true

	for _, p := range params {

		paramName, err := p.GetName()
		if err != nil {
			return nil, err
		}

		kind, err := p.GetKind()
		if err != nil {
			return nil, err
		}

		isDefaultVal, err := p.GetIsDefaultValue()
		if err != nil {
			return nil, err
		}

		var pyty string

		if kind == "KEYWORD_ONLY" {
			// skip the parameter
			// add "dict_named_args" parameter instead

			// if *all* keyword_only parameters are optional, make dict_named_args optional

			// if dict_named_args is optional - add it last
			// if dict_named_args is NOT optional - add after last non-optional parameter

			isAdd_dict_named_args = true
			if isAllKeywordOnlyOptional && !isDefaultVal {
				isAllKeywordOnlyOptional = false
			}

			pyty, err = p.GetType()
			if err != nil {
				return nil, err
			}

		} else if kind == "VAR_KEYWORD" {
			// replace with "dict_var_args" parameter instead
			pyty = "dict"

		} else if kind == "VAR_POSITIONAL" {
			// replace with "list_var_args" parameter instead
			pyty = "list"
		} else {
			pyty, err = p.GetType()
			if err != nil {
				return nil, err
			}
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

		mffiparam := IDL.NewArgArrayDefinitionWithAlias(paramName, mffiType, dims, talias)
		mffiparam.IsOptional = isDefaultVal

		pyfunc.AddParameter(mffiparam)
	}

	// add dict parameter for "Keyword Only" parameters
	if isAdd_dict_named_args {
		p := IDL.NewArgArrayDefinitionWithAlias("dict_named_args", IDL.HANDLE, 0, "dict")
		if isAllKeywordOnlyOptional {
			// add as last parameter, and optional
			p.IsOptional = true
			pyfunc.AddParameter(p)
		} else {
			p.IsOptional = false
			i := pyfunc.GetFirstIndexOfOptionalParameter()

			// insert after last optional parameter
			pyfunc.Parameters = append(pyfunc.Parameters[:i+1], pyfunc.Parameters[i:]...)
			pyfunc.Parameters[i] = p
		}
	}

	for i, pyty := range retvals {
		retvalname := fmt.Sprintf("ret_%d", i)

		mffiType := pyTypeToMFFI(pyty)
		var talias string
		if mffiType == IDL.HANDLE || mffiType == IDL.HANDLE_ARRAY {
			talias = pyty
		}

		dims := 0
		if pyty == "list" {
			dims = 1
		}

		pyfunc.AddReturnValues(IDL.NewArgArrayDefinitionWithAlias(retvalname, mffiType, dims, talias))
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
