#!/usr/bin/env python3
"""
Simple test to verify that the JVM IDL plugin can be loaded and initialized.
"""

import os
import sys
import subprocess

def test_plugin_load():
    """Test if the plugin can be loaded without crashing"""
    
    print("Testing JVM IDL plugin load...")
    
    # Check if METAFFI_HOME is set
    metaffi_home = os.getenv('METAFFI_HOME')
    if not metaffi_home:
        print("ERROR: METAFFI_HOME environment variable not set")
        return False
    
    print(f"METAFFI_HOME: {metaffi_home}")
    
    # Check if the plugin DLL exists
    plugin_dll = os.path.join(metaffi_home, "jvm", "metaffi.idl.jvm.dll")
    if not os.path.exists(plugin_dll):
        print(f"ERROR: Plugin DLL not found: {plugin_dll}")
        return False
    
    print(f"Plugin DLL found: {plugin_dll}")
    
    # Check if the JAR file exists
    jar_file = os.path.join(metaffi_home, "jvm", "jvm_idl_extractor.jar")
    if not os.path.exists(jar_file):
        print(f"ERROR: JAR file not found: {jar_file}")
        return False
    
    print(f"JAR file found: {jar_file}")
    
    # Try to run a simple metaffi command that should load the plugin
    try:
        cmd = ["metaffi", "-c", "--idl-plugin", "jvm", "--idl", "test.jar", "-h", "python3", "--print-idl"]
        print(f"Running command: {' '.join(cmd)}")
        
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        
        print(f"Exit code: {result.returncode}")
        print(f"stdout: {result.stdout}")
        print(f"stderr: {result.stderr}")
        
        if result.returncode == 3221225477:
            print("ERROR: Plugin crashed with access violation (exit code 3221225477)")
            return False
        elif result.returncode != 0:
            print(f"ERROR: Command failed with exit code {result.returncode}")
            return False
        else:
            print("SUCCESS: Plugin loaded and ran without crashing")
            return True
            
    except subprocess.TimeoutExpired:
        print("ERROR: Command timed out")
        return False
    except Exception as e:
        print(f"ERROR: Exception occurred: {e}")
        return False

if __name__ == "__main__":
    success = test_plugin_load()
    sys.exit(0 if success else 1) 
