#!/usr/bin/env python3

import sys
import os
import tempfile
import unittest
import json
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from py_extractor import py_extractor
from py_idl_generator import PyIDLGenerator

class TestPyIDLGenerator(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.mkdtemp()
    def tearDown(self):
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    def create_test_file(self, content, filename="test_file.py"):
        filepath = os.path.join(self.temp_dir, filename)
        with open(filepath, "w") as f:
            f.write(content)
        return filepath
    def test_simple_function(self):
        code = '''
def add(x: int, y: int) -> int:
    """Add two numbers"""
    return x + y
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        pyinfo = extractor.extract()
        generator = PyIDLGenerator(filepath)
        idl_json = generator.generate_json(pyinfo)
        print("\n[test_simple_function] Generated JSON:\n", idl_json)
        idl = json.loads(idl_json)
        self.assertIn("modules", idl)
        module = idl["modules"][0]
        self.assertEqual(module["name"], "test_file")
        self.assertEqual(module["target_language"], "python3")
        found = False
        for func in module["functions"]:
            if func["name"] == "add":
                found = True
                self.assertEqual(func["comment"], "Add two numbers")
                self.assertEqual(func["parameters"][0]["name"], "x")
                self.assertEqual(func["parameters"][0]["type"], "INT32")
                self.assertEqual(func["parameters"][1]["name"], "y")
                self.assertEqual(func["parameters"][1]["type"], "INT32")
                self.assertEqual(func["return_values"][0]["type"], "INT32")
        self.assertTrue(found)
    def test_class_and_global(self):
        code = '''
class Counter:
    """A simple counter"""
    def __init__(self, start: int = 0):
        self.value = start
    def increment(self) -> int:
        self.value += 1
        return self.value

MY_CONST = 123
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        pyinfo = extractor.extract()
        generator = PyIDLGenerator(filepath)
        idl_json = generator.generate_json(pyinfo)
        print("\n[test_class_and_global] Generated JSON:\n", idl_json)
        idl = json.loads(idl_json)
        module = idl["modules"][0]
        # Check class
        found_class = False
        for cls in module["classes"]:
            if cls["name"] == "Counter":
                found_class = True
                self.assertEqual(cls["comment"], "A simple counter")
                self.assertEqual(cls["constructors"][0]["name"], "__init__")
        self.assertTrue(found_class)
        # Check global
        found_global = False
        for g in module["functions"]:
            if g["name"].startswith("GetMY_CONST") or g["name"].startswith("SetMY_CONST"):
                found_global = True
        self.assertTrue(found_global)
    def test_multiple_functions_and_types(self):
        code = '''
def foo(a: str, b: float) -> str:
    return a + str(b)
def bar(x: list) -> list:
    return x
def baz() -> None:
    pass
'''
        filepath = self.create_test_file(code)
        extractor = py_extractor(filepath)
        pyinfo = extractor.extract()
        generator = PyIDLGenerator(filepath)
        idl_json = generator.generate_json(pyinfo)
        print("\n[test_multiple_functions_and_types] Generated JSON:\n", idl_json)
        idl = json.loads(idl_json)
        module = idl["modules"][0]
        func_names = [f["name"] for f in module["functions"]]
        self.assertIn("foo", func_names)
        self.assertIn("bar", func_names)
        self.assertIn("baz", func_names)
        for func in module["functions"]:
            if func["name"] == "foo":
                self.assertEqual(func["parameters"][0]["type"], "STRING8")
                self.assertEqual(func["parameters"][1]["type"], "FLOAT64")
                self.assertEqual(func["return_values"][0]["type"], "STRING8")
            if func["name"] == "bar":
                self.assertEqual(func["parameters"][0]["type"], "HANDLE")
                self.assertEqual(func["return_values"][0]["type"], "HANDLE")
            if func["name"] == "baz":
                self.assertEqual(len(func["return_values"]), 0)

if __name__ == "__main__":
    unittest.main() 