package main

import (
	"fmt"
	. "github.com/MetaFFI/lang-plugin-python3/idl/py_extractor"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
)

var classes map[string]*IDL.ClassDefinition

// --------------------------------------------------------------------
func ExtractClasses(pyinfo *Py_info, metaffiGuestLib string) ([]*IDL.ClassDefinition, error) {

	classes = make(map[string]*IDL.ClassDefinition)

	pyclses, err := pyinfo.GetClasses()
	if err != nil {
		return nil, err
	}

	for _, c := range pyclses {
		clsName, err := c.GetName()
		if err != nil {
			return nil, err
		}

		pycls := IDL.NewClassDefinition(clsName)

		comment, err := c.GetComment()
		if err != nil {
			return nil, err
		}
		pycls.Comment = comment

		fields, err := c.GetFields()
		if err != nil {
			return nil, err
		}

		for _, f := range fields {
			name, err := f.GetName()
			if err != nil {
				return nil, err
			}

			typy, err := f.GetType()
			if err != nil {
				return nil, err
			}

			isGetter, err := f.GetIsGetter()
			if err != nil {
				return nil, err
			}

			isSetter, err := f.GetIsSetter()
			if err != nil {
				return nil, err
			}

			if isGetter && isSetter {
				fdecl := IDL.NewFieldDefinition(pycls, name, pyTypeToMFFI(typy), "Get"+name, "Set"+name, true)

				fdecl.Getter.SetTag("receiver_pointer", "true")
				fdecl.Getter.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
				fdecl.Getter.SetFunctionPath("entrypoint_function", "EntryPoint_"+pycls.Name+"_"+fdecl.Getter.Name)

				fdecl.Setter.SetTag("receiver_pointer", "true")
				fdecl.Setter.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
				fdecl.Setter.SetFunctionPath("entrypoint_function", "EntryPoint_"+pycls.Name+"_"+fdecl.Setter.Name)

				pycls.AddField(fdecl)

			} else if isGetter {
				fdecl := IDL.NewFieldDefinition(pycls, name, pyTypeToMFFI(typy), "Get"+name, "", true)

				fdecl.Getter.SetTag("receiver_pointer", "true")
				fdecl.Getter.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
				fdecl.Getter.SetFunctionPath("entrypoint_function", "EntryPoint_"+pycls.Name+"_"+fdecl.Getter.Name)

				pycls.AddField(fdecl)

			} else if isSetter {
				fdecl := IDL.NewFieldDefinition(pycls, name, pyTypeToMFFI(typy), "", "Set"+name, true)

				fdecl.Setter.SetTag("receiver_pointer", "true")
				fdecl.Setter.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
				fdecl.Setter.SetFunctionPath("entrypoint_function", "EntryPoint_"+pycls.Name+"_"+fdecl.Setter.Name)

				pycls.AddField(fdecl)
			} else {
				return nil, fmt.Errorf("Field/property %v.%v has neither getter or setter", clsName, name)
			}
		}

		pymethods, err := c.GetMethods()
		if err != nil {
			return nil, err
		}

		for _, f := range pymethods {

			name, err := f.GetName()
			if err != nil {
				return nil, err
			}

			comment, err := f.GetComment()
			if err != nil {
				return nil, err
			}

			params, err := f.GetParameters()
			if err != nil {
				return nil, err
			}

			if len(params) > 1 {
				params = params[1:]
			} else {
				params = []Parameter_info{}
			}

			retvals, err := f.GetReturnValues()
			if err != nil {
				return nil, err
			}

			pymeth, err := GenerateFunctionDefinition(name, comment, params, retvals, metaffiGuestLib)
			if err != nil {
				return nil, err
			}

			if name == "__init__" {
				pymeth.Name = clsName
				cstr := IDL.NewConstructorDefinitionFromFunctionDefinition(pymeth)
				cstr.SetFunctionPath("entrypoint_function", "EntryPoint_"+pycls.Name+"_"+cstr.Name)
				cstr.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
				pycls.AddConstructor(cstr)
			} else if name == "__del__" {
				pycls.Releaser = IDL.NewReleaserDefinitionFromFunctionDefinition(pycls, pymeth)
			} else {
				meth := IDL.NewMethodDefinitionWithFunction(pycls, pymeth, true)
				meth.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
				meth.SetFunctionPath("entrypoint_function", "EntryPoint_"+pycls.Name+"_"+meth.Name)
				pycls.AddMethod(meth)
			}
		}

		classes[pycls.Name] = pycls
	}

	res := make([]*IDL.ClassDefinition, 0)
	for _, c := range classes {

		c.Releaser.SetFunctionPath("metaffi_guest_lib", metaffiGuestLib)
		c.Releaser.SetFunctionPath("entrypoint_function", "EntryPoint_"+c.Name+"_"+c.Releaser.Name)

		res = append(res, c)
	}

	return res, nil
}

//--------------------------------------------------------------------
