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
type HostCompiler struct{
	def *compiler.IDLDefinition
	outputDir string
	serializationCode map[string]string
	hostOptions map[string]string
	outputFilename string
}
//--------------------------------------------------------------------
func NewHostCompiler(definition *compiler.IDLDefinition, outputDir string, outputFilename string, serializationCode map[string]string, hostOptions map[string]string) *HostCompiler{

	serializationCodeCopy := make(map[string]string)
	for k, v := range serializationCode{
		serializationCodeCopy[k] = v
	}

	return &HostCompiler{def: definition,
		outputDir: outputDir,
		serializationCode: serializationCodeCopy,
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
	outputFileName = this.outputDir+string(os.PathSeparator)+this.outputFilename+"_OpenFFIHost_pb2.py"
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

		"ToPythonType": func (elem string) string{
			pyType, found := OpenFFITypeToPython3Type[elem]
			if !found{
				panic("Type "+elem+" is not an OpenFFI type")
			}

			return pyType
		},
	}

	tmp, err := template.New("host").Funcs(funcMap).Parse(HostFunctionStubsTemplate)
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

