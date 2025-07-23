package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
	"text/template"

	compiler "github.com/MetaFFI/plugin-sdk/compiler/go"
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
)

var pythonKeywords = map[string]bool{
	"False":    true,
	"None":     true,
	"True":     true,
	"and":      true,
	"as":       true,
	"assert":   true,
	"async":    true,
	"await":    true,
	"break":    true,
	"class":    true,
	"continue": true,
	"def":      true,
	"del":      true,
	"elif":     true,
	"else":     true,
	"except":   true,
	"finally":  true,
	"for":      true,
	"from":     true,
	"global":   true,
	"if":       true,
	"import":   true,
	"in":       true,
	"is":       true,
	"lambda":   true,
	"nonlocal": true,
	"not":      true,
	"or":       true,
	"pass":     true,
	"raise":    true,
	"return":   true,
	"try":      true,
	"while":    true,
	"with":     true,
	"yield":    true,
}

// --------------------------------------------------------------------
type HostCompiler struct {
	def            *IDL.IDLDefinition
	outputDir      string
	hostOptions    map[string]string
	outputFilename string
}

// --------------------------------------------------------------------
func NewHostCompiler() *HostCompiler {
	return &HostCompiler{}
}

// --------------------------------------------------------------------
func (this *HostCompiler) Compile(definition *IDL.IDLDefinition, outputDir string, outputFilename string, hostOptions map[string]string) (err error) {

	compiler.ModifyKeywords(definition, pythonKeywords, func(keyword string) string { return keyword + "__" })

	if outputFilename == "" {
		outputFilename = definition.IDLSource
	}

	outputFilename = strings.ReplaceAll(outputFilename, filepath.Ext(outputFilename), "")
	outputFilename = strings.ReplaceAll(outputFilename, ".", "_") // filename must not contains "."

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

// --------------------------------------------------------------------
func (this *HostCompiler) parseHeader() (string, error) {
	tmp, err := template.New("host").Parse(HostHeaderTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse HostHeaderTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}

// --------------------------------------------------------------------
func (this *HostCompiler) parseMetaFFIImports() (string, error) {
	tmp, err := template.New("MetaFFIImports").Parse(MetaFFIImportsTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse MetaFFIImportsTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}

// --------------------------------------------------------------------
func (this *HostCompiler) parseRuntimeInitialization() (string, error) {
	tmp, err := template.New("RuntimeInitialization").Funcs(templatesFuncMap).Parse(RuntimeInitializationTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse RuntimeInitializationTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}

// --------------------------------------------------------------------
func (this *HostCompiler) parseFunctions() (string, error) {
	tmp, err := template.New("Functions").Funcs(templatesFuncMap).Parse(FunctionsTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse FunctionsTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}

// --------------------------------------------------------------------
func (this *HostCompiler) parseClasses() (string, error) {
	tmp, err := template.New("Classes").Funcs(templatesFuncMap).Parse(ClassesTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse ClassesTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}

// --------------------------------------------------------------------
func (this *HostCompiler) parseGlobals() (string, error) {
	tmp, err := template.New("Globals").Funcs(templatesFuncMap).Parse(GlobalsTemplate)
	if err != nil {
		return "", fmt.Errorf("Failed to parse GlobalsTemplate: %v", err)
	}

	buf := strings.Builder{}
	err = tmp.Execute(&buf, this.def)

	return buf.String(), err
}

// --------------------------------------------------------------------
func (this *HostCompiler) generateCode() (string, error) {

	header, err := this.parseHeader()
	if err != nil {
		return "", err
	}

	imports, err := this.parseMetaFFIImports()
	if err != nil {
		return "", err
	}

	runtimeInit, err := this.parseRuntimeInitialization()
	if err != nil {
		return "", err
	}

	functions, err := this.parseFunctions()
	if err != nil {
		return "", err
	}

	classes, err := this.parseClasses()
	if err != nil {
		return "", err
	}

	globals, err := this.parseGlobals()
	if err != nil {
		return "", err
	}

	res := header + imports + runtimeInit + functions + classes + globals

	return res, nil
}

//--------------------------------------------------------------------
