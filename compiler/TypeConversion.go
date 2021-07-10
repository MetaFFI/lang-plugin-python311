package main

import "github.com/OpenFFI/plugin-sdk/compiler/go"

var OpenFFITypeToPython3Type = map[compiler.OpenFFIType]string{

	compiler.FLOAT64: "float",
	compiler.FLOAT32: "float",

	compiler.INT8: "int",
	compiler.INT16: "int",
	compiler.INT32: "int",
	compiler.INT64: "int",

	compiler.UINT8: "int",
	compiler.UINT16: "int",
	compiler.UINT32: "int",
	compiler.UINT64: "int",

	compiler.BOOL: "bool",

	compiler.STRING8: "str",
	compiler.STRING16: "str",
	compiler.STRING32: "str",

	compiler.SIZE: "int",
}