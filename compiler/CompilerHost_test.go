package main

import (
	compiler "github.com/OpenFFI/plugin-sdk/compiler/go"
	"os"
	"os/exec"
	"testing"
)

const idl_host = `{"idl_filename": "test","idl_extension": ".proto","idl_filename_with_extension": "test.proto","idl_full_path": "","modules": [{"name": "Service1","target_language": "test","comment": "Comments for Service1\n","tags": {"openffi_function_path": "package=main","openffi_target_language": "python3"},"functions": [{"name": "f1","comment": "f1 comment\nparam1 comment\n","tags": {"openffi_function_path": "function=f1"},"path_to_foreign_function": {"module": "$PWD/temp","package": "GoFuncs","function": "f1"},"parameter_type": "Params1","return_values_type": "Return1","parameters": [{ "name": "p1", "type": "float64", "comment": "= 3.141592", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p2", "type": "float32", "comment": "= 2.71", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p3", "type": "int8", "comment": "= -10", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p4", "type": "int16", "comment": "= -20", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p5", "type": "int32", "comment": "= -30", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p6", "type": "int64", "comment": "= -40", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p7", "type": "uint8", "comment": "= 50", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p8", "type": "uint16", "comment": "= 60", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p9", "type": "uint32", "comment": "= 70", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p10", "type": "uint64", "comment": "= 80", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p11", "type": "bool", "comment": "= true", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p12", "type": "string", "comment": "= This is an input", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p13", "type": "string", "comment": "= {element one, element two}", "tags": null, "is_array": true, "pass_method": "" },{ "name": "p14", "type": "uint8", "comment": "= {2, 4, 6, 8, 10}", "tags": null, "is_array": true, "pass_method": "" }],"return_values": [{"name": "r1","type": "string","comment": "= {return one, return two}","tags": null,"is_array": true,"pass_method": ""}]}]}]}`
const compilerTestCode = `
from test_OpenFFIHost import *

def host_compiler():

	res = f1(3.141592, 2.71, -10, -20, -30, -40, 50, 60, 70, 80, True, 'This is an input', ["element one", "element two"], [2, 4, 6, 8, 10])
	
	if len(res) != 2:
		raise RuntimeError("Expected result length is 2. Received: "+str(len(res)))

	if res[0] != "return one":
		raise RuntimeError('Expected res[0] is "return one". Received: '+res[0])

	if res[1] != "return two":
		raise RuntimeError('Expected res[1] is "return two". Received: '+res[1])

host_compiler()
print('Host test passed successfully')
`

//--------------------------------------------------------------------
func TestHost(t *testing.T){

	def, err := compiler.NewIDLDefinition(idl_host)
	if err != nil{
		t.Fatal(err)
		return
	}

	_ = os.RemoveAll("temp")

	err = os.Mkdir("temp", 0700)
	if err != nil{
		t.Fatal(err)
		return
	}
/*
	defer func(){
		err = os.RemoveAll("temp")
		if err != nil{
			t.Fatal(err)
			return
		}
	}()
*/
	cmp := NewCompiler(def, "./temp")
	_, err = cmp.CompileHost(nil)
	if err != nil{
		t.Fatal(err)
		return
	}

	err = os.WriteFile("./temp/CompilerTestCode_test.py", []byte(compilerTestCode), 0700)
	if err != nil {
		t.Fatal(err)
		return
	}

	buildCmd := exec.Command("python3", "CompilerTestCode_test.py")
	buildCmd.Dir = "./temp"
	output, err := buildCmd.CombinedOutput()
	if err != nil{
		println(string(output))
		t.Fatalf("Host test failed: %v.\nOutput:\n%v", err, string(output))
	}

}
//--------------------------------------------------------------------