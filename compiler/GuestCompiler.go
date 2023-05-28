package main

import (
	"fmt"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
	"text/template"
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
func (this *GuestCompiler) Compile(definition *IDL.IDLDefinition, outputDir string, outputFilename string, blockName string, blockCode string) (err error) {

	if outputFilename == "" {
		outputFilename = definition.IDLFilename
	}

	outputFilename = strings.ReplaceAll(outputFilename, filepath.Ext(outputFilename), "")

	this.def = definition
	this.outputDir = outputDir
	this.outputFilename = outputFilename

	// generate code
	code, err := this.generateCode()
	if err != nil {
		return fmt.Errorf("Failed to generate guest code: %v", err)
	}

	// write to output
	outputFullFileName := fmt.Sprintf("%v%v%v_MetaFFIGuest.py", this.outputDir, string(os.PathSeparator), this.outputFilename)
	err = ioutil.WriteFile(outputFullFileName, []byte(code), 0600)
	if err != nil {
		return fmt.Errorf("Failed to write dynamic library to %v. Error: %v", this.outputDir+this.outputFilename, err)
	}

	return nil

}

// --------------------------------------------------------------------
func (this *GuestCompiler) parseHeader() (string, error) {
	tmp, err := template.New("GuestHeaderTemplate").Parse(GuestHeaderTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse GuestHeaderTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}

// --------------------------------------------------------------------
func (this *GuestCompiler) parseForeignFunctions() (string, error) {

	tmpEntryPoint, err := template.New("GuestFunctionXLLRTemplate").Funcs(templatesFuncMap).Parse(GuestFunctionXLLRTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse GuestFunctionXLLRTemplate: %v", err)
	}

	bufEntryPoint := strings.Builder{}
	err = tmpEntryPoint.Execute(&bufEntryPoint, this.def)

	return bufEntryPoint.String(), err
}

// --------------------------------------------------------------------
func (this *GuestCompiler) parseImports() (string, error) {

	// get all imports from the def file
	imports := struct {
		Imports []string
	}{
		Imports: make([]string, 0),
	}

	set := make(map[string]bool)

	for _, m := range this.def.Modules {
		importMod := m.GetFunctionPathSet("module")
		for _, im := range importMod {
			set[im] = true
		}
	}

	for _, m := range this.def.Modules {
		for _, e := range m.ExternalResources {
			importMod := os.ExpandEnv(e)
			set[importMod] = true
		}
	}

	for k, _ := range set {
		imports.Imports = append(imports.Imports, k)
	}

	tmp, err := template.New("GuestImportsTemplate").Parse(GuestImportsTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse GuestImportsTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, imports)
	importsCode := buf.String()

	return importsCode, err
}

// --------------------------------------------------------------------
func (this *GuestCompiler) generateCode() (string, error) {

	header, err := this.parseHeader()
	if err != nil {
		return "", err
	}

	imports, err := this.parseImports()
	if err != nil {
		return "", err
	}

	functionStubs, err := this.parseForeignFunctions()
	if err != nil {
		return "", err
	}

	res := header + imports + GuestHelperFunctions + functionStubs

	return res, nil
}

//--------------------------------------------------------------------
