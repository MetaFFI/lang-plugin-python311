#!/usr/bin/env python3

import sys
import os
import tempfile

# Add the current directory to the path so we can import py_extractor
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from py_extractor import py_extractor

def test_fixed_extractor():
    """Test the fixed py_extractor functionality"""
    
    # Create a simple test Python file
    test_code = '''
import time

def hello_world():
    """A simple hello world function"""
    print('Hello World, from Python3')

def returns_an_error():
    raise Exception('Error')

def div_integers(x: int, y: int) -> float:
    return x / y

def join_strings(arr: list) -> str:
    res = ','.join(arr)
    return res

five_seconds = 5

def wait_a_bit(secs: int) -> None:
    time.sleep(secs)
    return None

class testmap:
    name: str
    curdict: dict

    def __init__(self):
        self.curdict = dict()

    def set(self, k: str, v: any):
        self.curdict[k] = v

    def get(self, k: str) -> any:
        return self.curdict[k]

    def contains(self, k: str) -> bool:
        return k in self.curdict
'''
    
    # Write test file
    test_file = "test_file.py"
    with open(test_file, "w") as f:
        f.write(test_code)
    
    try:
        # Test the extractor
        extractor = py_extractor(test_file)
        result = extractor.extract()
        
        print("=== FIXED PY_EXTRACTOR TEST RESULTS ===")
        print(f"Globals: {len(result.globals)}")
        for g in result.globals:
            print(f"  - {g}")
        
        print(f"Functions: {len(result.functions)}")
        for f in result.functions:
            print(f"  - {f.name}: {f.parameters} -> {f.return_values}")
            if f.comment:
                print(f"    Comment: {f.comment}")
        
        print(f"Classes: {len(result.classes)}")
        for c in result.classes:
            print(f"  - {c.name}: {len(c.methods)} methods, {len(c.fields)} fields")
            if c.comment:
                print(f"    Comment: {c.comment}")
            for m in c.methods:
                print(f"    - {m.name}: {m.parameters} -> {m.return_values}")
                if m.comment:
                    print(f"      Comment: {m.comment}")
        
        return result
        
    except Exception as e:
        print(f"Error testing py_extractor: {e}")
        import traceback
        traceback.print_exc()
        return None
    finally:
        # Clean up
        if os.path.exists(test_file):
            os.remove(test_file)

if __name__ == "__main__":
    test_fixed_extractor() 