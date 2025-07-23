#!/usr/bin/env python3

import json
import os
import re
from typing import Dict, List, Any, Optional
from py_extractor import py_info, variable_info, function_info, class_info, parameter_info

class PyIDLGenerator:
    """Converts py_extractor output to MetaFFI JSON IDL format"""
    
    def __init__(self, source_path: str):
        self.source_path = source_path
        self.idl_filename = os.path.basename(source_path)
        self.idl_extension = os.path.splitext(self.idl_filename)[1]
        self.idl_filename_with_extension = self.idl_filename
        self.idl_full_path = os.path.abspath(source_path)
        
    def py_type_to_metaffi_type(self, py_type: str) -> str:
        """Convert Python type to MetaFFI type"""
        type_mapping = {
            'int': 'INT32',
            'float': 'FLOAT64',
            'str': 'STRING8',
            'bool': 'BOOLEAN',
            'list': 'HANDLE',
            'dict': 'HANDLE',
            'tuple': 'HANDLE',
            'set': 'HANDLE',
            'any': 'HANDLE',
            'None': 'HANDLE',
            'object': 'HANDLE',
            'complex': 'HANDLE',
            'bytes': 'HANDLE',
            'bytearray': 'HANDLE',
            'frozenset': 'HANDLE',
            'memoryview': 'HANDLE',
            'slice': 'HANDLE',
            'range': 'HANDLE',
            'type': 'HANDLE',
            'function': 'HANDLE',
            'method': 'HANDLE',
            'module': 'HANDLE',
            'class': 'HANDLE',
            'property': 'HANDLE',
            'staticmethod': 'HANDLE',
            'classmethod': 'HANDLE',
            'super': 'HANDLE',
            'object': 'HANDLE',
            'Exception': 'HANDLE',
            'BaseException': 'HANDLE',
            'Warning': 'HANDLE',
            'UserWarning': 'HANDLE',
            'DeprecationWarning': 'HANDLE',
            'PendingDeprecationWarning': 'HANDLE',
            'SyntaxWarning': 'HANDLE',
            'RuntimeWarning': 'HANDLE',
            'FutureWarning': 'HANDLE',
            'ImportWarning': 'HANDLE',
            'UnicodeWarning': 'HANDLE',
            'BytesWarning': 'HANDLE',
            'ResourceWarning': 'HANDLE',
        }
        
        # Clean up the type name
        clean_type = py_type.strip()
        if clean_type in type_mapping:
            return type_mapping[clean_type]
        
        # Handle complex types
        if '|' in clean_type or 'Union' in clean_type or 'Optional' in clean_type:
            return 'HANDLE'
        
        # Handle generic types
        if clean_type.startswith('List[') or clean_type.startswith('list['):
            return 'HANDLE_ARRAY'
        if clean_type.startswith('Dict[') or clean_type.startswith('dict['):
            return 'HANDLE'
        if clean_type.startswith('Tuple[') or clean_type.startswith('tuple['):
            return 'HANDLE'
        if clean_type.startswith('Set[') or clean_type.startswith('set['):
            return 'HANDLE'
        
        # Default to HANDLE for unknown types
        return 'HANDLE'
    
    def create_parameter_definition(self, param: parameter_info) -> Dict[str, Any]:
        """Create MetaFFI parameter definition"""
        # Handle case where parameter_info attributes might not be initialized
        param_name = getattr(param, 'name', 'param')
        param_type = getattr(param, 'type', 'any')
        
        metaffi_type = self.py_type_to_metaffi_type(param_type)
        
        param_def = {
            "name": param_name,
            "type": metaffi_type,
            "type_alias": param_type if metaffi_type in ['HANDLE', 'HANDLE_ARRAY'] else "",
            "comment": "",
            "tags": {},
            "dimensions": 0
        }
        
        return param_def
    
    def create_return_value_definition(self, return_type: str, index: int = 0) -> Dict[str, Any]:
        """Create MetaFFI return value definition"""
        metaffi_type = self.py_type_to_metaffi_type(return_type)
        
        return_def = {
            "name": f"ret_{index}",
            "type": metaffi_type,
            "type_alias": return_type if metaffi_type in ['HANDLE', 'HANDLE_ARRAY'] else "",
            "comment": "",
            "tags": {},
            "dimensions": 0
        }
        
        return return_def
    
    def create_function_definition(self, func: function_info, module_name: str) -> Dict[str, Any]:
        """Create MetaFFI function definition"""
        # Convert parameters
        parameters = []
        for param in func.parameters:
            parameters.append(self.create_parameter_definition(param))
        
        # Convert return values
        return_values = []
        for i, return_type in enumerate(func.return_values):
            return_values.append(self.create_return_value_definition(return_type, i))
        
        function_def = {
            "name": func.name,
            "comment": func.comment or "",
            "tags": {},
            "entity_path": {
                "module": module_name,
                "package": module_name
            },
            "parameters": parameters,
            "return_values": return_values
        }
        
        return function_def
    
    def create_global_definition(self, global_var: variable_info, module_name: str) -> Dict[str, Any]:
        """Create MetaFFI global definition"""
        metaffi_type = self.py_type_to_metaffi_type(global_var.type)
        
        # Create getter function
        getter_name = f"Get{global_var.name}"
        getter_def = {
            "name": getter_name,
            "comment": f"Get {global_var.name}",
            "tags": {},
            "entity_path": {
                "module": module_name,
                "package": module_name
            },
            "parameters": [],
            "return_values": [self.create_return_value_definition(global_var.type)]
        }
        
        # Create setter function
        setter_name = f"Set{global_var.name}"
        setter_def = {
            "name": setter_name,
            "comment": f"Set {global_var.name}",
            "tags": {},
            "entity_path": {
                "module": module_name,
                "package": module_name
            },
            "parameters": [self.create_parameter_definition(parameter_info())],
            "return_values": []
        }
        
        # Set parameter name for setter
        if setter_def["parameters"]:
            setter_def["parameters"][0]["name"] = "value"
            setter_def["parameters"][0]["type"] = metaffi_type
            setter_def["parameters"][0]["type_alias"] = global_var.type if metaffi_type in ['HANDLE', 'HANDLE_ARRAY'] else ""
        
        return {
            "getter": getter_def,
            "setter": setter_def
        }
    
    def create_constructor_definition(self, class_name: str, module_name: str) -> Dict[str, Any]:
        """Create MetaFFI constructor definition"""
        constructor_def = {
            "name": "__init__",
            "comment": f"Constructor for {class_name}",
            "tags": {},
            "entity_path": {
                "module": module_name,
                "package": module_name
            },
            "parameters": [],
            "return_values": [self.create_return_value_definition(class_name)]
        }
        
        return constructor_def
    
    def create_method_definition(self, method: function_info, class_name: str, module_name: str) -> Dict[str, Any]:
        """Create MetaFFI method definition"""
        # Convert parameters (skip 'self' for methods)
        parameters = []
        for param in method.parameters:
            if param.name != 'self':
                parameters.append(self.create_parameter_definition(param))
        
        # Convert return values
        return_values = []
        for i, return_type in enumerate(method.return_values):
            return_values.append(self.create_return_value_definition(return_type, i))
        
        method_def = {
            "name": method.name,
            "comment": method.comment or "",
            "tags": {},
            "entity_path": {
                "module": module_name,
                "package": module_name
            },
            "parameters": parameters,
            "return_values": return_values
        }
        
        return method_def
    
    def create_class_definition(self, cls: class_info, module_name: str) -> Dict[str, Any]:
        """Create MetaFFI class definition"""
        # Find constructor
        constructor = None
        methods = []
        
        for method in cls.methods:
            if method.name == "__init__":
                constructor = self.create_constructor_definition(cls.name, module_name)
                # Copy parameters from the actual __init__ method
                for param in method.parameters:
                    if param.name != 'self':
                        constructor["parameters"].append(self.create_parameter_definition(param))
            else:
                methods.append(self.create_method_definition(method, cls.name, module_name))
        
        # If no constructor found, create default one
        if constructor is None:
            constructor = self.create_constructor_definition(cls.name, module_name)
        
        # Create release method
        release_def = {
            "name": "release",
            "comment": f"Release {cls.name}",
            "tags": {},
            "entity_path": {
                "module": module_name,
                "package": module_name
            },
            "parameters": [self.create_parameter_definition(parameter_info())],
            "return_values": []
        }
        
        # Set parameter for release method
        if release_def["parameters"]:
            release_def["parameters"][0]["name"] = "self"
            release_def["parameters"][0]["type"] = "HANDLE"
            release_def["parameters"][0]["type_alias"] = cls.name
        
        # Convert class fields
        fields = []
        for field in cls.fields:
            field_def = {
                "name": field.name,
                "type": self.py_type_to_metaffi_type(field.type),
                "type_alias": "",
                "comment": "",
                "tags": {},
                "dimensions": 0
            }
            fields.append(field_def)
        
        class_def = {
            "name": cls.name,
            "comment": cls.comment or "",
            "tags": {},
            "constructors": [constructor],
            "release": release_def,
            "methods": methods,
            "fields": fields,
            "constants": []
        }
        
        return class_def
    
    def generate_idl(self, py_info: py_info) -> Dict[str, Any]:
        """Generate MetaFFI IDL JSON from py_info"""
        # Determine module name
        module_name = os.path.splitext(self.idl_filename)[0]
        
        # Convert functions
        functions = []
        for func in py_info.functions:
            functions.append(self.create_function_definition(func, module_name))
        
        # Convert globals (flatten getter/setter into functions)
        for global_var in py_info.globals:
            global_defs = self.create_global_definition(global_var, module_name)
            functions.append(global_defs["getter"])
            functions.append(global_defs["setter"])
        
        # Convert classes
        classes = []
        for cls in py_info.classes:
            classes.append(self.create_class_definition(cls, module_name))
        
        # Create module
        module = {
            "name": module_name,
            "target_language": "python3",
            "comment": f"Generated from {self.idl_filename}",
            "tags": {},
            "functions": functions,
            "classes": classes,
            "constants": [],
            "external_resources": [module_name]
        }
        
        # Create IDL definition
        idl_def = {
            "idl_filename": self.idl_filename,
            "idl_extension": self.idl_extension,
            "idl_filename_with_extension": self.idl_filename_with_extension,
            "idl_full_path": self.idl_full_path,
            "modules": [module]
        }
        
        return idl_def
    
    def generate_json(self, py_info: py_info) -> str:
        """Generate MetaFFI IDL JSON string"""
        idl_def = self.generate_idl(py_info)
        return json.dumps(idl_def, indent=2)


def generate_idl_from_py_info(py_info: py_info, source_path: str) -> str:
    """Convenience function to generate IDL JSON from py_info"""
    generator = PyIDLGenerator(source_path)
    return generator.generate_json(py_info)


if __name__ == "__main__":
    import sys
    from py_extractor import py_extractor
    
    if len(sys.argv) != 2:
        print("Usage: python py_idl_generator.py <python_file>")
        sys.exit(1)
    
    source_file = sys.argv[1]
    
    # Extract entities
    extractor = py_extractor(source_file)
    result = extractor.extract()
    
    # Generate IDL
    generator = PyIDLGenerator(source_file)
    json_output = generator.generate_json(result)
    
    print("Generated MetaFFI IDL JSON:")
    print(json_output) 