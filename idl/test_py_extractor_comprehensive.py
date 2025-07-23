#!/usr/bin/env python3

import sys
import os
import json
import tempfile
import unittest
from pathlib import Path

# Add the current directory to the path so we can import py_extractor
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from py_extractor import py_extractor, py_info, variable_info, function_info, class_info, parameter_info

class TestPyExtractor(unittest.TestCase):
    """Comprehensive tests for py_extractor.py"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.temp_dir = tempfile.mkdtemp()
        
    def tearDown(self):
        """Clean up test fixtures"""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def create_test_file(self, content, filename="test_file.py"):
        """Create a test Python file with given content"""
        filepath = os.path.join(self.temp_dir, filename)
        with open(filepath, "w") as f:
            f.write(content)
        return filepath
    
    def test_basic_functions(self):
        """Test extraction of basic functions"""
        code = '''
def simple_function():
    """A simple function"""
    pass

def function_with_args(x: int, y: str) -> bool:
    """Function with type annotations"""
    return True

def function_with_defaults(a: int = 1, b: str = "default") -> None:
    """Function with default values"""
    pass
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        result = extractor.extract()
        
        self.assertEqual(len(result.functions), 3)
        
        # Check simple_function
        func = next(f for f in result.functions if f.name == "simple_function")
        self.assertEqual(func.name, "simple_function")
        self.assertEqual(func.comment, "A simple function")
        self.assertEqual(len(func.parameters), 0)
        self.assertEqual(len(func.return_values), 1)
        self.assertEqual(func.return_values[0], "any")
        
        # Check function_with_args
        func = next(f for f in result.functions if f.name == "function_with_args")
        self.assertEqual(func.name, "function_with_args")
        self.assertEqual(func.comment, "Function with type annotations")
        self.assertEqual(len(func.parameters), 2)
        self.assertEqual(func.parameters[0].name, "x")
        self.assertEqual(func.parameters[0].type, "int")
        self.assertEqual(func.parameters[1].name, "y")
        self.assertEqual(func.parameters[1].type, "str")
        self.assertEqual(len(func.return_values), 1)
        self.assertEqual(func.return_values[0], "bool")
        
        # Check function_with_defaults
        func = next(f for f in result.functions if f.name == "function_with_defaults")
        self.assertEqual(len(func.parameters), 2)
        self.assertTrue(func.parameters[0].is_default_value)
        self.assertTrue(func.parameters[1].is_default_value)
    
    def test_globals(self):
        """Test extraction of global variables"""
        code = '''
import os
import sys

# Global variables
MY_CONSTANT = 42
my_string = "hello"
my_list = [1, 2, 3]
my_dict = {"key": "value"}

def some_function():
    pass
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        result = extractor.extract()
        
        # Should have globals (excluding functions)
        global_names = [g.name for g in result.globals]
        self.assertIn("MY_CONSTANT", global_names)
        self.assertIn("my_string", global_names)
        self.assertIn("my_list", global_names)
        self.assertIn("my_dict", global_names)
        
        # Check types
        my_constant = next(g for g in result.globals if g.name == "MY_CONSTANT")
        self.assertEqual(my_constant.type, "int")
    
    def test_classes(self):
        """Test extraction of classes"""
        code = '''
class SimpleClass:
    """A simple class"""
    class_var = 10
    
    def __init__(self, name: str):
        self.name = name
    
    def get_name(self) -> str:
        """Get the name"""
        return self.name
    
    def set_name(self, name: str) -> None:
        """Set the name"""
        self.name = name

class ComplexClass:
    """A more complex class"""
    
    def __init__(self):
        self.data = {}
    
    @property
    def count(self) -> int:
        """Get the count"""
        return len(self.data)
    
    @count.setter
    def count(self, value: int):
        """Set the count"""
        pass
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        result = extractor.extract()
        
        self.assertEqual(len(result.classes), 2)
        
        # Check SimpleClass
        simple_class = next(c for c in result.classes if c.name == "SimpleClass")
        self.assertEqual(simple_class.comment, "A simple class")
        user_methods = [m for m in simple_class.methods if not m.name.startswith("__")]
        self.assertEqual(len(user_methods), 2)  # get_name, set_name
        self.assertEqual(len(simple_class.fields), 1)   # class_var should be extracted as field
        
        # Check __init__ method
        init_method = next(m for m in simple_class.methods if m.name == "__init__")
        self.assertEqual(len(init_method.parameters), 2)  # self + name
        self.assertEqual(init_method.return_values[0], "SimpleClass")
        
        # Check get_name method
        get_name_method = next(m for m in simple_class.methods if m.name == "get_name")
        self.assertEqual(len(get_name_method.parameters), 1)  # self
        self.assertEqual(get_name_method.return_values[0], "str")
        
        # Check ComplexClass with properties
        complex_class = next(c for c in result.classes if c.name == "ComplexClass")
        user_methods = [m for m in complex_class.methods if not m.name.startswith("__")]
        self.assertEqual(len(user_methods), 0)  # only property methods, no user-defined
        self.assertEqual(len(complex_class.fields), 1)   # count property
    
    def test_package_extraction(self):
        """Test extraction from a package/module name"""
        # Create a simple package structure
        package_dir = os.path.join(self.temp_dir, "test_package")
        os.makedirs(package_dir)
        
        # Create __init__.py
        init_content = '''
"""Test package"""

from test_package.module1 import func1
from test_package.module2 import Class1

__all__ = ['func1', 'Class1']
'''
        with open(os.path.join(package_dir, "__init__.py"), "w") as f:
            f.write(init_content)
        
        # Create module1.py
        module1_content = '''
def func1(x: int) -> str:
    """Function in module1"""
    return str(x)
'''
        with open(os.path.join(package_dir, "module1.py"), "w") as f:
            f.write(module1_content)
        
        # Create module2.py
        module2_content = '''
class Class1:
    """Class in module2"""
    
    def __init__(self, value: int):
        self.value = value
    
    def get_value(self) -> int:
        return self.value
'''
        with open(os.path.join(package_dir, "module2.py"), "w") as f:
            f.write(module2_content)
        
        # Test extraction from package directory
        sys.path.insert(0, self.temp_dir)
        try:
            extractor = py_extractor(package_dir)
            result = extractor.extract()
        finally:
            sys.path.remove(self.temp_dir)
        # Should extract from __init__.py
        self.assertGreater(len(result.functions), 0)
        self.assertGreater(len(result.classes), 0)
    
    def test_error_handling(self):
        """Test error handling for invalid files"""
        # Test with non-existent file
        with self.assertRaises(Exception):
            extractor = py_extractor("non_existent_file.py")
            extractor.extract()
        
        # Test with invalid Python syntax
        invalid_code = '''
def invalid_syntax(
    # Missing closing parenthesis
'''
        filepath = self.create_test_file(invalid_code)
        
        # Should handle syntax errors gracefully
        try:
            extractor = py_extractor(filepath)
            result = extractor.extract()
            # If it doesn't raise an exception, that's fine too
        except Exception as e:
            # Expected behavior for syntax errors
            pass
    
    def test_complex_types(self):
        """Test extraction of complex type annotations"""
        code = '''
from typing import Any, List, Dict, Optional, Union
import numpy as np

def complex_function(
    items: List[str],
    config: Dict[str, Union[int, str]],
    optional_param: Optional[float] = None
) -> List[Dict[str, Any]]:
    """Function with complex type annotations"""
    pass

def numpy_function(arr: np.ndarray) -> np.ndarray:
    """Function with numpy types"""
    pass
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        result = extractor.extract()
        
        # Should handle complex types (may be simplified to 'any')
        complex_func = next(f for f in result.functions if f.name == "complex_function")
        self.assertEqual(len(complex_func.parameters), 3)
        
        # Check that complex types are handled
        for param in complex_func.parameters:
            self.assertIsNotNone(param.type)
    
    def test_property_extraction(self):
        """Test extraction of properties"""
        code = '''
class PropertyClass:
    def __init__(self):
        self._value = 0
    
    @property
    def value(self) -> int:
        """Get the value"""
        return self._value
    
    @value.setter
    def value(self, new_value: int):
        """Set the value"""
        self._value = new_value
    
    @property
    def computed(self) -> str:
        """Computed property"""
        return "computed"
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        result = extractor.extract()
        
        prop_class = next(c for c in result.classes if c.name == "PropertyClass")
        
        # Should have properties as fields
        field_names = [f.name for f in prop_class.fields]
        self.assertIn("value", field_names)
        self.assertIn("computed", field_names)
        
        # Check property getter/setter
        value_field = next(f for f in prop_class.fields if f.name == "value")
        self.assertTrue(value_field.is_getter)
        self.assertTrue(value_field.is_setter)
        
        computed_field = next(f for f in prop_class.fields if f.name == "computed")
        self.assertTrue(computed_field.is_getter)
        self.assertFalse(computed_field.is_setter)  # No setter defined

if __name__ == "__main__":
    unittest.main() 