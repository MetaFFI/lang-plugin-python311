package main

import (
	compiler "github.com/OpenFFI/plugin-sdk/compiler/go"
	"io/ioutil"

	//	compiler "github.com/OpenFFI/plugin-sdk/compiler/go"
//	"os"
	"os"
	"testing"
)

const idl_guest = `{"idl_filename": "test","idl_extension": ".proto","idl_filename_with_extension": "test.proto","idl_full_path": "","modules": [{"name": "Service1","target_language": "test","comment": "Comments for Service1\n","tags": {"openffi_function_path": "package=main","openffi_target_language": "python3"},"functions": [{"name": "f1","comment": "f1 comment\nparam1 comment\n","tags": {"openffi_function_path": "function=f1"},"path_to_foreign_function": {"module": "GuestCode","package": "GoFuncs","function": "f1"},"parameter_type": "Params1","return_values_type": "Return1","parameters": [{ "name": "p1", "type": "float64", "comment": "= 3.141592", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p2", "type": "float32", "comment": "= 2.71", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p3", "type": "int8", "comment": "= -10", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p4", "type": "int16", "comment": "= -20", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p5", "type": "int32", "comment": "= -30", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p6", "type": "int64", "comment": "= -40", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p7", "type": "uint8", "comment": "= 50", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p8", "type": "uint16", "comment": "= 60", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p9", "type": "uint32", "comment": "= 70", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p10", "type": "uint64", "comment": "= 80", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p11", "type": "bool", "comment": "= true", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p12", "type": "string", "comment": "= This is an input", "tags": null, "is_array": false, "pass_method": "" },{ "name": "p13", "type": "string", "comment": "= {element one, element two}", "tags": null, "is_array": true, "pass_method": "" },{ "name": "p14", "type": "uint8", "comment": "= {2, 4, 6, 8, 10}", "tags": null, "is_array": true, "pass_method": "" }],"return_values": [{"name": "r1","type": "string","comment": "= {return one, return two}","tags": null,"is_array": true,"pass_method": ""}]}]}]}`

const GuestCode = `
from typing import List

def f1(p1: float, p2: float, p3: int, p4: int, p5: int, p6: int, p7: int, p8: int, p9: int, p10: int, p11: bool, p12: str, p13: List[str], p14: List[int])-> List[str]:

	""" This function expects the parameters (in that order):
		double = 3.141592
	    float = 2.71f

	    int8 = -10
	    int16 = -20
	    int32 = -30
	    int64 = -40

	    uint8 = 50
	    uint16 = 60
	    uint32 = 70
	    uint64 = 80

	    bool = 1

	    string = "This is an input"
	    string[] = {"element one", "element two"}

	    bytes = {2, 4, 6, 8, 10}
	"""

	print("Hello from Python F1")

	if p1 != 3.141592:
		raise RuntimeError("p1 != 3.141592. p1 = "+str(p1))
	

	if p2 - 2.71 > 0.000001: # float in python are actually double, therefore using C-float (32bit) "looses" its accuracy as python-float (64bit)
		raise RuntimeError("p2 - 2.71 > 0.000001. p2 = "+str(p2))

	if p3 != -10:
		raise RuntimeError("p3 != -10. p3 = "+str(p3))
	

	if p4 != -20:
		raise RuntimeError("p4 != -20")
	

	if p5 != -30:
		raise RuntimeError("p5 != -30")
	

	if p6 != -40:
		raise RuntimeError("p6 != -40")
	

	if p7 != 50:
		raise RuntimeError("p7 != 50")
	

	if p8 != 60:
		raise RuntimeError("p8 != 60")
	

	if p9 != 70:
		raise RuntimeError("p9 != 70")
	

	if p10 != 80:
		raise RuntimeError("p10 != 80")
	

	if not p11:
		raise RuntimeError("p11 == false")
	

	if p12 != "This is an input":
		raise RuntimeError("p12 != \"This is an input\"")
	

	if len(p13) != 2:
		raise RuntimeError("len(p13) != 2")


	if p13[0] != "element one":
		raise RuntimeError("p13[0] != \"element one\"")


	if p13[1] != "element two":
		raise RuntimeError("p13[1] != \"element two\"")
	

	if len(p14) != 5:
		raise RuntimeError("len(p14) != 5")

	if p14[0] != 2 or p14[1] != 4 or p14[2] != 6 or p14[3] != 8 or p14[4] != 10:
		raise RuntimeError("p14[0] != 2 or p14[1] != 4 or p14[2] != 6 or p14[3] != 8 or p14[4] != 10")

	return ["return one", "return two"]
`

//--------------------------------------------------------------------
func TestGuest(t *testing.T){

	def, err := compiler.NewIDLDefinition(idl_guest)
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

	defer func(){
		err = os.RemoveAll("temp")
		if err != nil{
			t.Fatal(err)
			return
		}
	}()

	err = ioutil.WriteFile("./temp/GuestCode.py", []byte(GuestCode), 0600)
	if err != nil {
		t.Fatal(err)
		return
	}

	cmp := NewCompiler(def, "temp")
	_, err = cmp.CompileGuest()
	if err != nil{
		t.Fatal(err)
		return
	}

	err = os.Chdir("temp")
	if err != nil{
		t.Fatal(err)
	}

	defer func(){
		err = os.Chdir("..")
		if err != nil{
			t.Fatal(err)
			return
		}
	}()

	if CallHostMock() != 0{
		t.Fatal("Failed calling guest")
	}
}
//--------------------------------------------------------------------