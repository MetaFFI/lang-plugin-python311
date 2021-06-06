package main

import (
	"fmt"
	compiler "github.com/OpenFFI/plugin-sdk/compiler/go"
	"html/template"
	"io/ioutil"
	"os"
	"strings"
)

//--------------------------------------------------------------------
type GuestCompiler struct{
	def *compiler.IDLDefinition
	outputDir string
	outputFilename string
}
//--------------------------------------------------------------------
func NewGuestCompiler(definition *compiler.IDLDefinition, outputDir string, outputFilename string) *GuestCompiler{

	return &GuestCompiler{def: definition, outputDir: outputDir, outputFilename: outputFilename}
}
//--------------------------------------------------------------------
func (this *GuestCompiler) Compile() (outputFileName string, err error){

	// generate code
	code, err := this.generateCode()
	if err != nil{
		return "", fmt.Errorf("Failed to generate guest code: %v", err)
	}

	// write to output
	outputFullFileName := fmt.Sprintf("%v%v%v_OpenFFIGuest.py", this.outputDir, string(os.PathSeparator), this.outputFilename)
	err = ioutil.WriteFile(outputFullFileName, []byte(code), 0600)
	if err != nil{
		return "", fmt.Errorf("Failed to write dynamic library to %v. Error: %v", this.outputDir+this.outputFilename, err)
	}

	return outputFullFileName, nil

}
//--------------------------------------------------------------------
func (this *GuestCompiler) parseHeader() (string, error){
	tmp, err := template.New("guest").Parse(GuestHeaderTemplate)
	if err != nil{
		return "", fmt.Errorf("Failed to parse GuestHeaderTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}
//--------------------------------------------------------------------
func (this *GuestCompiler) parseForeignFunctions() (string, error){

	var funcMap = map[string]interface{}{
		"AsPublic": func(elem string) string {
			if len(elem) == 0 {
				return ""
			} else if len(elem) == 1 {
				return strings.ToUpper(elem)
			} else {
				return strings.ToUpper(elem[0:1]) + elem[1:]
			}
		},
	}

	tmpEntryPoint, err := template.New("guest").Funcs(funcMap).Parse(GuestFunctionXLLRTemplate)
	if err != nil{
		return "", fmt.Errorf("Failed to parse GuestFunctionXLLRTemplate: %v", err)
	}

	bufEntryPoint := strings.Builder{}
	err = tmpEntryPoint.Execute(&bufEntryPoint, this.def)

	return bufEntryPoint.String(), err
}
//--------------------------------------------------------------------
func (this *GuestCompiler) parseImports() (string, error){

	// get all imports from the def file
	imports := struct {
		Imports []string
	}{
		Imports: make([]string, 0),
	}

	set := make(map[string]bool)

	for _, m := range this.def.Modules{
		for _, f := range m.Functions{
			if mod, found := f.PathToForeignFunction["module"]; found{

				set[mod] = true
			}
		}
	}

	for k, _ := range set{
		imports.Imports = append(imports.Imports, k)
	}

	tmp, err := template.New("guest").Parse(GuestImportsTemplate)
	if err != nil{
		return "", fmt.Errorf("Failed to parse GuestImportsTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, imports)
	importsCode := buf.String()

	return importsCode, err
}
//--------------------------------------------------------------------
func (this *GuestCompiler) generateCode() (string, error){

	header, err := this.parseHeader()
	if err != nil{ return "", err }

	imports, err := this.parseImports()
	if err != nil{ return "", err }

	functionStubs, err := this.parseForeignFunctions()
	if err != nil{ return "", err }

	res := header + imports + functionStubs

	return res, nil
}
//--------------------------------------------------------------------
