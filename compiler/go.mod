module github.com/MetaFFI/lang-plugin-python3/compiler

go 1.23.0

toolchain go1.24.4

require (
	github.com/MetaFFI/sdk/compiler/go v0.0.0
	github.com/MetaFFI/sdk/idl_entities/go v0.0.0
)

replace github.com/MetaFFI/sdk/compiler/go => ../../sdk/compiler/go

replace github.com/MetaFFI/sdk/idl_entities/go => ../../sdk/idl_entities/go
