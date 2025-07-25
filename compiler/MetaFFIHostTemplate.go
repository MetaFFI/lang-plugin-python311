package main

const MetaFFIHostHeaderTemplate = `
# Code generated by MetaFFI. DO NOT EDIT.
# Host code for {{.IDLFilenameWithExtension}}
`

const MetaFFIImportsTemplate = `
import metaffi
from metaffi import MetaFFITypes
from typing import List, Any, Optional
import platform
`

const RuntimeInitializationTemplate = `
# Initialize MetaFFI runtime
runtime = None
module = None

def initialize_runtime(target_language: str = "{{.TargetLanguage}}", module_path: str = "{{range .Modules}}{{.Name}}{{end}}_MetaFFIGuest"):
    global runtime, module
    if runtime is None:
        runtime = metaffi.metaffi_runtime.MetaFFIRuntime(target_language)
        runtime.load_runtime_plugin()
        if platform.system() == 'Windows':
            module = runtime.load_module(module_path + '.dll')
        elif platform.system() == 'Linux':
            module = runtime.load_module(module_path + '.so')
        else:
            raise Exception(f'Unsupported system {platform.system()}')
    return runtime, module

# Auto-initialize on import
initialize_runtime()
`

const FunctionsTemplate = `
{{range .Modules}}
# Functions for module {{.Name}}
{{range .Functions}}
def {{.Name}}({{range $index, $param := .Parameters}}{{if $index}}, {{end}}{{$param.Name}}: {{ConvertToPythonType $param.Type (IsArray $param.Dimensions)}}{{end}}) -> {{if .ReturnValues}}{{range $ret := .ReturnValues}}{{ConvertToPythonType $ret.Type (IsArray $ret.Dimensions)}}{{end}}{{else}}None{{end}}:
    """
    Call foreign function {{.Name}}
    """
    try:
        # Load entity if not already loaded
        entity = module.load_entity("callable={{.Name}}", 
                                   [{{range $index, $param := .Parameters}}{{if $index}}, {{end}}metaffi.metaffi_types.metaffi_type_info({{GetMetaFFITypeForAPI $param}}){{end}}],
                                   [{{range $index, $ret := .ReturnValues}}{{if $index}}, {{end}}metaffi.metaffi_types.metaffi_type_info({{GetMetaFFITypeForAPI $ret}}){{end}}])
        
        # Call the foreign function
        {{if .ReturnValues}}result = {{end}}entity({{range $index, $param := .Parameters}}{{if $index}}, {{end}}{{$param.Name}}{{end}})
        {{if .ReturnValues}}return result{{end}}
    except Exception as e:
        raise RuntimeError(f"Failed to call foreign function {{.Name}}: {e}")

{{end}}
{{end}}
`

const ClassesTemplate = `
{{range .Modules}}
# Classes for module {{.Name}}
{{range .Classes}}
class {{.Name}}:
    """
    Python wrapper for foreign class {{.Name}}
    """
    
    def __init__(self{{if .Constructors}}{{range $constructor := .Constructors}}{{range $index, $param := $constructor.Parameters}}, {{$param.Name}}: {{ConvertToPythonType $param.Type (IsArray $param.Dimensions)}}{{end}}{{end}}{{else}}, handle: Optional[Any] = None{{end}}):
        """
        Initialize {{.Name}} instance
        """
        try:
            {{if .Constructors}}
            # Load constructor entity
            constructor_entity = module.load_entity("{{.Name}}", 
                                                  [{{range $constructor := .Constructors}}{{range $index, $param := $constructor.Parameters}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $param}}{{end}}{{end}}],
                                                  [{{range $constructor := .Constructors}}{{range $index, $ret := $constructor.ReturnValues}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $ret}}{{end}}{{end}}])
            
            # Call constructor
            self.handle = constructor_entity({{range $constructor := .Constructors}}{{range $index, $param := $constructor.Parameters}}{{if $index}}, {{end}}{{$param.Name}}{{end}}{{end}})
            {{else}}
            self.handle = handle
            {{end}}
        except Exception as e:
            raise RuntimeError(f"Failed to initialize {{.Name}}: {e}")
    
    {{range .Methods}}
    def {{.Name}}(self{{range $index, $param := .Parameters}}{{if $index}}, {{end}}{{$param.Name}}: {{ConvertToPythonType $param.Type (IsArray $param.Dimensions)}}{{end}}) -> {{if .ReturnValues}}{{range $ret := .ReturnValues}}{{ConvertToPythonType $ret.Type (IsArray $ret.Dimensions)}}{{end}}{{else}}None{{end}}:
        """
        Call foreign method {{.Name}}
        """
        try:
            # Load method entity
            entity = module.load_entity("{{.Name}}", 
                                       [{{range $index, $param := .Parameters}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $param}}{{end}}],
                                       [{{range $index, $ret := .ReturnValues}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $ret}}{{end}}])
            
            # Call the method
            {{if .ReturnValues}}result = {{end}}entity(self.handle{{range $index, $param := .Parameters}}, {{$param.Name}}{{end}})
            {{if .ReturnValues}}return result{{end}}
        except Exception as e:
            raise RuntimeError(f"Failed to call method {{.Name}}: {e}")
    
    {{end}}
    
    {{range .Fields}}
    {{if .Getter}}
    @property
    def {{.Name}}(self) -> {{if .Getter.ReturnValues}}{{range $ret := .Getter.ReturnValues}}{{ConvertToPythonType $ret.Type (IsArray $ret.Dimensions)}}{{end}}{{else}}None{{end}}:
        """
        Getter for field {{.Name}}
        """
        try:
            # Load getter entity
            entity = module.load_entity("{{.Getter.Name}}", 
                                       [{{range $index, $param := .Getter.Parameters}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $param}}{{end}}],
                                       [{{range $index, $ret := .Getter.ReturnValues}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $ret}}{{end}}])
            
            # Call the getter
            return entity(self.handle{{range $index, $param := .Getter.Parameters}}, {{$param.Name}}{{end}})
        except Exception as e:
            raise RuntimeError(f"Failed to get field {{.Name}}: {e}")
    
    {{end}}
    {{if .Setter}}
    @{{.Name}}.setter
    def {{.Name}}(self, value: {{range $index, $param := .Setter.Parameters}}{{if eq $index 1}}{{ConvertToPythonType $param.Type (IsArray $param.Dimensions)}}{{end}}{{end}}):
        """
        Setter for field {{.Name}}
        """
        try:
            # Load setter entity
            entity = module.load_entity("{{.Setter.Name}}", 
                                       [{{range $index, $param := .Setter.Parameters}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $param}}{{end}}],
                                       [{{range $index, $ret := .Setter.ReturnValues}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $ret}}{{end}}])
            
            # Call the setter
            entity(self.handle, value)
        except Exception as e:
            raise RuntimeError(f"Failed to set field {{.Name}}: {e}")
    
    {{end}}
    {{end}}
    
    {{if .Releaser}}
    def __del__(self):
        """
        Cleanup foreign object
        """
        try:
            if hasattr(self, 'handle') and self.handle is not None:
                # Load releaser entity
                entity = module.load_entity("{{.Releaser.Name}}", 
                                           [{{range $index, $param := .Releaser.Parameters}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $param}}{{end}}],
                                           [{{range $index, $ret := .Releaser.ReturnValues}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $ret}}{{end}}])
                
                # Call the releaser
                entity(self.handle)
        except Exception:
            # Ignore cleanup errors
            pass
    
    {{end}}
{{end}}
{{end}}
`

const GlobalsTemplate = `
{{range .Modules}}
# Global variables for module {{.Name}}
{{range .Globals}}
{{if .Getter}}
def get_{{.Name}}() -> {{if .Getter.ReturnValues}}{{range $ret := .Getter.ReturnValues}}{{ConvertToPythonType $ret.Type (IsArray $ret.Dimensions)}}{{end}}{{else}}None{{end}}:
    """
    Getter for global variable {{.Name}}
    """
    try:
        # Load getter entity
        entity = module.load_entity("global={{.Name}},getter", 
                                   [{{range $index, $param := .Getter.Parameters}}{{if $index}}, {{end}}metaffi.metaffi_types.metaffi_type_info({{GetMetaFFITypeForAPI $param}}){{end}}],
                                   [{{range $index, $ret := .Getter.ReturnValues}}{{if $index}}, {{end}}metaffi.metaffi_types.metaffi_type_info({{GetMetaFFITypeForAPI $ret}}){{end}}])
        
        # Call the getter
        return entity({{range $index, $param := .Getter.Parameters}}{{if $index}}, {{end}}{{$param.Name}}{{end}})
    except Exception as e:
        raise RuntimeError(f"Failed to get global {{.Name}}: {e}")

{{end}}
{{if .Setter}}
def set_{{.Name}}(value: {{range $index, $param := .Setter.Parameters}}{{if eq $index 1}}{{ConvertToPythonType $param.Type (IsArray $param.Dimensions)}}{{end}}{{end}}):
    """
    Setter for global variable {{.Name}}
    """
    try:
        # Load setter entity
        entity = module.load_entity("{{.Setter.Name}}", 
                                   [{{range $index, $param := .Setter.Parameters}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $param}}{{end}}],
                                   [{{range $index, $ret := .Setter.ReturnValues}}{{if $index}}, {{end}}{{GetMetaFFITypeForAPI $ret}}{{end}}])
        
        # Call the setter
        entity({{range $index, $param := .Setter.Parameters}}{{if $index}}, {{end}}{{$param.Name}}{{end}})
    except Exception as e:
        raise RuntimeError(f"Failed to set global {{.Name}}: {e}")

{{end}}
{{end}}
{{end}}
`
