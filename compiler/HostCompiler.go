package main

import (
	"fmt"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
	"text/template"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

//--------------------------------------------------------------------
type HostCompiler struct{
	def *IDL.IDLDefinition
	outputDir string
	hostOptions map[string]string
	outputFilename string
}
//--------------------------------------------------------------------
func NewHostCompiler(definition *IDL.IDLDefinition, outputDir string, outputFilename string, hostOptions map[string]string) *HostCompiler{

	if strings.Contains(outputFilename, "#"){
		toRemove := outputFilename[strings.LastIndex(outputFilename, string(os.PathSeparator))+1:strings.Index(outputFilename, "#")+1]
		outputFilename = strings.ReplaceAll(outputFilename, toRemove, "")
	}

	outputFilename = strings.ReplaceAll(outputFilename, filepath.Ext(outputFilename), "")

	return &HostCompiler{def: definition,
		outputDir: outputDir,
		hostOptions: hostOptions,
		outputFilename: outputFilename}
}
//--------------------------------------------------------------------
func (this *HostCompiler) Compile() (outputFileName string, err error){

	// generate code
	code, err := this.generateCode()
	if err != nil{
		return "", fmt.Errorf("Failed to generate guest code: %v", err)
	}

	// write to output
	outputFileName = this.outputDir+string(os.PathSeparator)+this.outputFilename+"_MetaFFIHost.py"
	err = ioutil.WriteFile( outputFileName, []byte(code), 0600)
	if err != nil{
		return "", fmt.Errorf("Failed to write host code to %v. Error: %v", this.outputDir+this.outputFilename, err)
	}

	return outputFileName, nil

}
//--------------------------------------------------------------------
func (this *HostCompiler) parseHeader() (string, error){
	tmp, err := template.New("host").Parse(HostHeaderTemplate)
	if err != nil{
		return "", fmt.Errorf("Failed to parse HostHeaderTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}
//--------------------------------------------------------------------
func (this *HostCompiler) parseForeignStubs() (string, error){

	tmp, err := template.New("host").Funcs(templatesFuncMap).Parse(HostFunctionStubsTemplate)
	if err != nil{
		return "", fmt.Errorf("Failed to parse HostFunctionStubsTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}
//--------------------------------------------------------------------
func (this *HostCompiler) generateCode() (string, error){

	header, err := this.parseHeader()
	if err != nil{ return "", err }

	functionStubs, err := this.parseForeignStubs()
	if err != nil{ return "", err }

	res := header + HostImports + HostHelperFunctions + functionStubs

	return res, nil
}
//--------------------------------------------------------------------

