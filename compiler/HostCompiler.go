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

//--------------------------------------------------------------------
type HostCompiler struct {
	def            *IDL.IDLDefinition
	outputDir      string
	hostOptions    map[string]string
	outputFilename string
}

//--------------------------------------------------------------------
func NewHostCompiler() *HostCompiler {
	return &HostCompiler{}
}

//--------------------------------------------------------------------
func (this *HostCompiler) Compile(definition *IDL.IDLDefinition, outputDir string, outputFilename string, hostOptions map[string]string) (err error) {

	if outputFilename == ""{
        outputFilename = definition.IDLFilename
    }

	if strings.Contains(outputFilename, "#") {
		toRemove := outputFilename[strings.LastIndex(outputFilename, string(os.PathSeparator))+1 : strings.Index(outputFilename, "#")+1]
		outputFilename = strings.ReplaceAll(outputFilename, toRemove, "")
	}
	
	outputFilename = strings.ReplaceAll(outputFilename, filepath.Ext(outputFilename), "")
	
	this.def = definition
	this.outputDir = outputDir
	this.hostOptions = hostOptions
	this.outputFilename = outputFilename

	// generate code
	code, err := this.generateCode()
	if err != nil {
		return fmt.Errorf("Failed to generate host code: %v", err)
	}
	
	// write to output
	genOutputFileName := this.outputDir + string(os.PathSeparator) + this.outputFilename + "_MetaFFIHost.py"
	err = ioutil.WriteFile(genOutputFileName, []byte(code), 0600)
	if err != nil {
		return fmt.Errorf("Failed to write host code to %v. Error: %v", this.outputDir+this.outputFilename, err)
	}
	
	return nil
	
}

//--------------------------------------------------------------------
func (this *HostCompiler) parseHeader() (string, error) {
	tmp, err := template.New("host").Parse(HostHeaderTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse HostHeaderTemplate: %v", err)
	}
	
	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)
	
	return buf.String(), err
}

//--------------------------------------------------------------------
func (this *HostCompiler) parseForeignStubs() (string, error) {
	
	tmp, err := template.New("Python HostFunctionStubsTemplate").Funcs(templatesFuncMap).Parse(HostFunctionStubsTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse Python HostFunctionStubsTemplate: %v", err)
	}
	
	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)
	
	return buf.String(), err
}

//--------------------------------------------------------------------
func (this *HostCompiler) generateCode() (string, error) {
	
	header, err := this.parseHeader()
	if err != nil {
		return "", err
	}
	
	functionStubs, err := this.parseForeignStubs()
	if err != nil {
		return "", err
	}
	
	res := header + HostImports + HostHelperFunctions + functionStubs
	
	return res, nil
}

//--------------------------------------------------------------------
