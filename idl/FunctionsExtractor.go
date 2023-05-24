package main

import (
	"fmt"
	"math"

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

		mffiType := pyTypeToMFFI(pyty)
		var talias string
		if mffiType == IDL.HANDLE || mffiType == IDL.HANDLE_ARRAY {
			talias = pyty
		}

		dims := 0
		if pyty == "list" {
			dims = 1
		}

		pyfunc.AddParameter(IDL.NewArgArrayDefinitionWithAlias(name, mffiType, dims, talias))
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
func GeneratePermutations[T any](input []T, permuteStartIndex int) [][]T {

	if permuteStartIndex >= len(input) {
		return [][]T{input}
	}

	res := make([][]T, 0)

	sizeOfPermutations := int(math.Pow(2, float64(len(input)-permuteStartIndex)))

	for i := 0; i < sizeOfPermutations; i++ {

		permutation := make([]T, 0)

		// add celles that appear in all permutations
		for k, item := range input {
			if k >= permuteStartIndex {
				break
			}

			permutation = append(permutation, item)
		}

		// for each i bit that is set, choose the i element
		for j := 0; j < i; j++ {
			if i&(1<<j) > 0 { // // if j-th bit is set
				permutation = append(permutation, input[permuteStartIndex+j])
			}
		}

		if len(permutation) > 0 {
			res = append(res, permutation)
		}
	}

	return res
}

// --------------------------------------------------------------------
func ExtractOverloadedParameters(params []Parameter_info) ([][]Parameter_info, error) {

	// detect how many options are there
	startingIndexOfOptionals := len(params)
	for i, p := range params {
		isDefault, err := p.GetIsDefaultValue()
		if err != nil {
			return nil, err
		}

		if isDefault {
			startingIndexOfOptionals = i
			break
		}
	}

	res := GeneratePermutations(params, startingIndexOfOptionals)

	return res, nil
}

// --------------------------------------------------------------------
func ExtractFunctions(pyinfo *Py_info, metaffiGuestLib string) ([]*IDL.FunctionDefinition, error) {

	functions := make([]*IDL.FunctionDefinition, 0)

	funcs, err := pyinfo.GetFunctions()
	if err != nil {
		return nil, err
	}

	for _, f := range funcs {

		originalParameters, err := f.GetParameters()
		if err != nil {
			return nil, err
		}

		// optional parameters generates multiple functions (overloads)
		overloadedParameters, err := ExtractOverloadedParameters(originalParameters)
		if err != nil {
			return nil, err
		}

		for _, params := range overloadedParameters {
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
	}

	return functions, nil
}

//--------------------------------------------------------------------
