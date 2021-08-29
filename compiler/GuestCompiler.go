package main

import (
	"fmt"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
	"html/template"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

//--------------------------------------------------------------------
type GuestCompiler struct{
	def *IDL.IDLDefinition
	outputDir string
	outputFilename string
}
//--------------------------------------------------------------------
func NewGuestCompiler(definition *IDL.IDLDefinition, outputDir string, outputFilename string) *GuestCompiler{

	if strings.Contains(outputFilename, "#"){
		toRemove := outputFilename[strings.LastIndex(outputFilename, string(os.PathSeparator))+1:strings.Index(outputFilename, "#")+1]
		outputFilename = strings.ReplaceAll(outputFilename, toRemove, "")
	}

	outputFilename = strings.ReplaceAll(outputFilename, filepath.Ext(outputFilename), "")

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
	outputFullFileName := fmt.Sprintf("%v%v%v_MetaFFIGuest.py", this.outputDir, string(os.PathSeparator), this.outputFilename)
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

	tmpEntryPoint, err := template.New("guest").Funcs(templatesFuncMap).Parse(GuestFunctionXLLRTemplate)
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
			if mod, found := f.FunctionPath["module"]; found{

				importMod := os.ExpandEnv(mod)
				set[importMod] = true
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

	res := header + imports + GuestHelperFunctions + functionStubs

	return res, nil
}
//--------------------------------------------------------------------
