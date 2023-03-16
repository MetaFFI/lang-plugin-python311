package main

import (
	"fmt"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
	. "github.com/MetaFFI/lang-plugin-python3/idl/py_extractor"
)

//--------------------------------------------------------------------
func ExtractFunctions(pyinfo *Py_info, metaffiGuestLib string) ([]*IDL.FunctionDefinition, error){

	functions := make([]*IDL.FunctionDefinition, 0)

	funcs, err := pyinfo.GetFunctions()
	if err != nil{ return nil, err }
	for _, f := range funcs{
		name, err := f.GetName()
		if err != nil{ return nil, err }

		pyfunc := IDL.NewFunctionDefinition(name)

		comment, err := f.GetComment()
		if err != nil{ return nil, err }
		pyfunc.Comment = comment

		params, err := f.GetParameters()
		if err != nil{ return nil, err }
		for _, p := range params{
			name, err := p.GetName()
			if err != nil{ return nil, err }

			pyty, err := p.GetType()
			if err != nil{ return nil, err }

			mffiType := pyTypeToMFFI(pyty)
			var talias string
			if mffiType == IDL.HANDLE || mffiType == IDL.HANDLE_ARRAY { talias = pyty }

			dims := 0
			if pyty == "list"{ dims = 1 }

			pyfunc.AddParameter(IDL.NewArgArrayDefinitionWithAlias(name, mffiType, dims, talias))
		}

		retvals, err := f.GetReturnValues()
		if err != nil{ return nil, err }
		for i, pyty := range retvals{
			name := fmt.Sprintf("ret_%d", i)

			mffiType := pyTypeToMFFI(pyty)
			var talias string
			if mffiType == IDL.HANDLE || mffiType == IDL.HANDLE_ARRAY { talias = pyty }

			dims := 0
			if pyty == "list"{ dims = 1 }

			pyfunc.AddReturnValues(IDL.NewArgArrayDefinitionWithAlias(name, mffiType, dims, talias))
		}
		
		pyfunc.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
		pyfunc.SetFunctionPath("entrypoint_function", "EntryPoint_"+pyfunc.Name)

		functions = append(functions, pyfunc)
	}

	return functions, nil
}
//--------------------------------------------------------------------
