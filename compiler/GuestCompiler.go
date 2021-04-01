package main

import (
	"fmt"
	compiler "github.com/OpenFFI/plugin-sdk/compiler/go"
	"html/template"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

//--------------------------------------------------------------------
type GuestCompiler struct{
	def *compiler.IDLDefinition
	outputDir string
	serializationCode map[string]string
	outputFilename string
}
//--------------------------------------------------------------------
func NewGuestCompiler(definition *compiler.IDLDefinition, outputDir string, outputFilename string, serializationCode map[string]string) *GuestCompiler{

	serializationCodeCopy := make(map[string]string)
	for k, v := range serializationCode{
		serializationCodeCopy[k] = v
	}

	return &GuestCompiler{def: definition, outputDir: outputDir, serializationCode: serializationCodeCopy, outputFilename: outputFilename}
}
//--------------------------------------------------------------------
func (this *GuestCompiler) Compile() (outputFileName string, err error){

	// generate code
	code, err := this.generateCode()
	if err != nil{
		return "", fmt.Errorf("Failed to generate guest code: %v", err)
	}

	// write to output
	outputFullFileName := fmt.Sprintf("%v%v%v_OpenFFIGuest.go", this.outputDir, string(os.PathSeparator), this.outputFilename)
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

	tmpForeignFunctions, err := template.New("guest").Funcs(funcMap).Parse(GuestFunctionTemplate)
	if err != nil{
		return "", fmt.Errorf("Failed to parse tmpForeignFunctions: %v", err)
	}

	bufForeignFunctions := strings.Builder{}
	err = tmpForeignFunctions.Execute(&bufForeignFunctions, this.def)

	tmpEntryPoint, err := template.New("guest").Funcs(funcMap).Parse(GuestFunctionXLLRTemplate)
	if err != nil{
		return "", fmt.Errorf("Failed to parse GuestFunctionXLLRTemplate: %v", err)
	}

	bufEntryPoint := strings.Builder{}
	err = tmpEntryPoint.Execute(&bufEntryPoint, this.def)

	return bufForeignFunctions.String() + "\n" + bufEntryPoint.String(), err
}
//--------------------------------------------------------------------
func (this *GuestCompiler) generateCode() (string, error){

	header, err := this.parseHeader()
	if err != nil{ return "", err }

	functionStubs, err := this.parseForeignFunctions()
	if err != nil{ return "", err }

	res := header + GuestImports + functionStubs

	// append serialization code in the same file
	for filename, serializationCode := range this.serializationCode{

		if strings.ToLower(filepath.Ext(filename)) != ".py"{
			continue
		}

		res += serializationCode
	}

	return res, nil
}
//--------------------------------------------------------------------
