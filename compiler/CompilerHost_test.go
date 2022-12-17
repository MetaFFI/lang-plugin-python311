package main

import (
	"github.com/MetaFFI/plugin-sdk/compiler/go/IDL"
	"os"
	"os/exec"
	"runtime"
	"testing"
)

const idl_host = `{"idl_filename": "test","idl_extension": ".json","idl_filename_with_extension": "test.json", "target_language": "test", "idl_full_path": "","modules": [{"name": "TestModule","comment": "Comments for TestModule\n","tags": null,"functions": [{"name": "f1","comment": "F1 comment\nparam1 comment\n","function_path": {"module": "$PWD/temp","package": "GoFuncs","function": "f1"},"parameter_type": "Params1","return_values_type": "Return1","parameters": [{"name": "p1","type": "float64","comment": "= 3.141592","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p2","type": "float32","comment": "= 2.71","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p3","type": "int8","comment": "= -10","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p4","type": "int16","comment": "= -20","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p5","type": "int32","comment": "= -30","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p6","type": "int64","comment": "= -40","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p7","type": "uint8","comment": "= 50","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p8","type": "uint16","comment": "= 60","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p9","type": "uint32","comment": "= 70","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p10","type": "uint64","comment": "= 80","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p11","type": "bool","comment": "= true","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p12","type": "string8","comment": "= This is an input","tags": null,"dimensions": 0,"pass_method": ""},{"name": "p13","type": "string8","comment": "= {element one, element two}","tags": null,"dimensions": 1,"pass_method": ""},{"name": "p14","type": "uint8","comment": "= {2, 4, 6, 8, 10}","tags": null,"dimensions": 1,"pass_method": ""}],"return_values": [{"name": "r1","type": "float64","comment": "= 0.57721","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r2","type": "float32","comment": "= 3.359","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r3","type": "int8","comment": "= -11","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r4","type": "int16","comment": "= -21","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r5","type": "int32","comment": "= -31","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r6","type": "int64","comment": "= -41","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r7","type": "uint8","comment": "= 51","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r8","type": "uint16","comment": "= 61","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r9","type": "uint32","comment": "= 71","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r10","type": "uint64","comment": "= 81","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r11","type": "bool","comment": "= true","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r12","type": "string8","comment": "= This is an output","tags": null,"dimensions": 0,"pass_method": ""},{"name": "r13","type": "string8","comment": "= {return one, return two}","tags": null,"dimensions": 1,"pass_method": ""},{"name": "r14","type": "uint8","comment": "= {20, 40, 60, 80, 100}","tags": null,"dimensions": 1,"pass_method": ""}]}]}]}`
const compilerTestCode = `
from test_MetaFFIHost import *

def host_compiler():

	res = f1(3.141592, 2.71, -10, -20, -30, -40, 50, 60, 70, 80, True, 'This is an input', ["element one", "element two"], [2, 4, 6, 8, 10])

	if len(res) != 14:
		raise RuntimeError("Expected result length is 2. Received: "+str(len(res)))

	if res[0] != 0.57721:
		raise RuntimeError('r1 != 0.57721')

	if res[1] - 3.359 > 0.000001:
		raise RuntimeError('r2 != 3.359, r2='+str(res[1]))

	if res[2] != -11:
		raise RuntimeError('r3 != -11')
	
	if res[3] != -21:
		raise RuntimeError('r4 != -21')
	
	if res[4] != -31:
		raise RuntimeError('r5 != -31')

	if res[5] != -41:
		raise RuntimeError('r6 != -41')

	if res[6] != 51:
		raise RuntimeError('r7 != 51')
	
	if res[7] != 61:
		raise RuntimeError('r8 != 61')
	
	if res[8] != 71:
		raise RuntimeError('r9 != 71')

	if res[9] != 81:
		raise RuntimeError('r10 != 81')

	if res[10] != True:
		raise RuntimeError('r11 != True')

	if res[11] != 'This is an output':
		raise RuntimeError('r12 != This is an output')

	if len(res[12]) != 2:
		raise RuntimeError('Length of given string[] expected to be 2, while it is '+str(len(res[12])))

	strarray = res[12]
	if strarray[0] != "return one":
		raise RuntimeError('strarray[0] != "return one"')

	if strarray[1] != "return two":
		raise RuntimeError('strarray[1] != "return two"')

	if len(res[13]) != 5:
		raise RuntimeError('Length of given uint8[] expected to be 5, while it is '+str(len(res[13])))

	barray = res[13]
	if barray[0] != 20:
		raise RuntimeError('barray[0] != 20')

	if barray[1] != 40:
		raise RuntimeError('barray[1] != 40')

	if barray[2] != 60:
		raise RuntimeError('barray[2] != 60')

	if barray[3] != 80:
		raise RuntimeError('barray[3] != 80')

	if barray[4] != 100:
		raise RuntimeError('barray[4] != 100')

host_compiler()
free_metaffi()
print('Host test passed successfully')
`

//--------------------------------------------------------------------
func TestHost(t *testing.T) {
	
	def, err := IDL.NewIDLDefinitionFromJSON(idl_host)
	if err != nil {
		t.Fatal(err)
		return
	}
	
	_ = os.RemoveAll("temp")
	
	err = os.Mkdir("temp", 0700)
	if err != nil {
		t.Fatal(err)
		return
	}

	defer func() {
		err = os.RemoveAll("temp")
		if err != nil {
			t.Fatal(err)
			return
		}
	}()

	cmp := NewHostCompiler()
	err = cmp.Compile(def, "./temp", "", nil)
	if err != nil {
		t.Fatal(err)
		return
	}
	
	err = os.WriteFile("./temp/CompilerTestCode_test.py", []byte(compilerTestCode), 0700)
	if err != nil {
		t.Fatal(err)
		return
	}
	
	var buildCmd *exec.Cmd
	if runtime.GOOS == "windows" {
		buildCmd = exec.Command("cmd.exe", "/c", "python3", "CompilerTestCode_test.py")
	} else {
		buildCmd = exec.Command("python3", "CompilerTestCode_test.py")
	}
	
	buildCmd.Dir = "./temp"
	
	output, err := buildCmd.CombinedOutput()
	if err != nil {
		println(string(output))
		t.Fatalf("Host test failed: %v.\nOutput:\n%v", err, string(output))
	}
	
}

//--------------------------------------------------------------------
