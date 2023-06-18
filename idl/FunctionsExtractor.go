package main

import (
	"fmt"
	. "github.com/MetaFFI/lang-plugin-python3/idl/py_extractor"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
)

// --------------------------------------------------------------------
func GenerateFunctionDefinition(name string, comment string, params []ParameterInfo, retvals []string, metaffiGuestLib string) (*IDL.FunctionDefinition, error) {

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

		isOptional, err := p.GetIsOptional()
        if err != nil {
            return nil, err
        }

        isOptional = isDefaultVal || isOptional

		var pyty string

		if kind == "KEYWORD_ONLY" {
			// skip the parameter
			// add "dict_named_args" parameter instead

			// if *all* keyword_only parameters are optional, make dict_named_args optional

			// if dict_named_args is optional - add it last
			// if dict_named_args is NOT optional - add after last non-optional parameter

			isAdd_dict_named_args = true
			if isAllKeywordOnlyOptional && !isOptional {
				isAllKeywordOnlyOptional = false
			}

			commentArgIsOptionalText := ""
			if isOptional{
				commentArgIsOptionalText = "(Optional)"
			}

			pyfunc.Comment += fmt.Sprintf("\nKeyword argument %v - %v", commentArgIsOptionalText, paramName)

			continue

		} else if kind == "VAR_KEYWORD" {
			// replace with "dict_var_args" parameter instead
			pyty = "dict"

			isOptional = true
			pyfunc.Comment += fmt.Sprintf("\nvariant keyword argument - %v", paramName)

		} else if kind == "VAR_POSITIONAL" {
			// replace with "list_var_args" parameter instead
			pyty = "list"

			isOptional = true
            pyfunc.Comment += fmt.Sprintf("\nvariant positional argument %v", paramName)

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

		mffiparam := IDL.NewArgArrayDefinitionWithAlias(paramName, mffiType, 0, talias)
		mffiparam.IsOptional = isOptional

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
			if i > -1{
				pyfunc.Parameters = append(pyfunc.Parameters[:i+1], pyfunc.Parameters[i:]...)
                pyfunc.Parameters[i] = p
			} else {
				pyfunc.Parameters = append(pyfunc.Parameters, p)
			}
		}
	}

	for i, pyty := range retvals {
		retvalname := fmt.Sprintf("ret_%d", i)

		mffiType := pyTypeToMFFI(pyty)
		var talias string
		if mffiType == IDL.HANDLE || mffiType == IDL.HANDLE_ARRAY {
			talias = pyty
		}

		pyfunc.AddReturnValues(IDL.NewArgArrayDefinitionWithAlias(retvalname, mffiType, 0, talias))
	}

	pyfunc.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
	pyfunc.SetFunctionPath("entrypoint_function", "EntryPoint_"+pyfunc.Name)

	return pyfunc, nil
}

// --------------------------------------------------------------------
func ExtractFunctions(pyinfo *PyInfo, metaffiGuestLib string) ([]*IDL.FunctionDefinition, error) {

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
