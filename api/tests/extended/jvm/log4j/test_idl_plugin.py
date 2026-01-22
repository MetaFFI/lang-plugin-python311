#!/usr/bin/env python3
"""
Test script to verify that the JVM IDL plugin can process JAR files.
This test checks if the IDL plugin is working correctly with log4j JAR files.
"""

import os
import sys
import subprocess
import json
import tempfile
import shutil


def run_command(command, cwd=None):
    """Run a command and return the result"""
    print(f"Running: {command}")
    try:
        result = subprocess.run(
            command, 
            shell=True, 
            cwd=cwd,
            capture_output=True, 
            text=True, 
            check=True
        )
        return result.stdout.strip(), result.stderr.strip(), 0
    except subprocess.CalledProcessError as e:
        return e.stdout.strip(), e.stderr.strip(), e.returncode


def test_idl_plugin_with_jars():
    """Test the JVM IDL plugin with log4j JAR files"""
    print("Testing JVM IDL plugin with log4j JAR files...")
    
    # Get the current directory (where the JAR files are)
    current_dir = os.path.dirname(os.path.abspath(__file__))
    jar_files = "log4j-api-2.21.1.jar;log4j-core-2.21.1.jar"
    
    # Test 1: Try to generate IDL from JAR files
    print("\nTest 1: Generating IDL from JAR files...")
    idl_command = f'metaffi -c --idl-plugin jvm --idl "{jar_files}" -h python3 --print-idl'
    stdout, stderr, returncode = run_command(idl_command, cwd=current_dir)
    
    if returncode == 0:
        print("✅ SUCCESS: IDL generation worked!")
        print("Generated IDL:")
        print(stdout)
        
        # Try to parse the IDL as JSON to verify it's valid
        try:
            idl_data = json.loads(stdout)
            print("✅ SUCCESS: Generated IDL is valid JSON")
            
            # Check if it contains expected log4j classes
            if 'entities' in idl_data:
                entities = idl_data['entities']
                log4j_classes = [e for e in entities if 'org.apache.logging.log4j' in str(e)]
                print(f"Found {len(log4j_classes)} log4j-related entities")
                
                if log4j_classes:
                    print("✅ SUCCESS: Found log4j classes in generated IDL")
                    return True
                else:
                    print("❌ WARNING: No log4j classes found in generated IDL")
                    return False
            else:
                print("❌ WARNING: No 'entities' field in generated IDL")
                return False
                
        except json.JSONDecodeError as e:
            print(f"❌ ERROR: Generated IDL is not valid JSON: {e}")
            return False
    else:
        print("❌ ERROR: IDL generation failed!")
        print(f"Return code: {returncode}")
        print(f"stdout: {stdout}")
        print(f"stderr: {stderr}")
        return False


def test_host_compiler_with_idl():
    """Test the Python311 host compiler with generated IDL"""
    print("\nTest 2: Testing Python311 host compiler...")
    
    # First generate IDL
    current_dir = os.path.dirname(os.path.abspath(__file__))
    jar_files = "log4j-api-2.21.1.jar;log4j-core-2.21.1.jar"
    
    idl_command = f'metaffi -c --idl-plugin jvm --idl "{jar_files}" -h python3 --print-idl'
    stdout, stderr, returncode = run_command(idl_command, cwd=current_dir)
    
    if returncode != 0:
        print("❌ ERROR: Cannot test host compiler - IDL generation failed")
        return False
    
    # Save IDL to a temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        f.write(stdout)
        idl_file = f.name
    
    try:
            # Test host compiler
    compiler_command = f"metaffi -c --idl={idl_file} -h python3"
    stdout, stderr, returncode = run_command(compiler_command, cwd=current_dir)
        
        if returncode == 0:
            print("✅ SUCCESS: Host compiler worked!")
            print("Generated Python code:")
            print(stdout)
            return True
        else:
            print("❌ ERROR: Host compiler failed!")
            print(f"Return code: {returncode}")
            print(f"stdout: {stdout}")
            print(f"stderr: {stderr}")
            return False
            
    finally:
        # Clean up temporary file
        os.unlink(idl_file)


def test_full_pipeline():
    """Test the full pipeline: IDL generation + host compilation"""
    print("\nTest 3: Testing full pipeline...")
    
    current_dir = os.path.dirname(os.path.abspath(__file__))
    jar_files = "log4j-api-2.21.1.jar;log4j-core-2.21.1.jar"
    
    # Test the full command that should generate Python code
    full_command = f"metaffi -c --idl-plugin jvm --idl {jar_files} -h python3"
    stdout, stderr, returncode = run_command(full_command, cwd=current_dir)
    
    if returncode == 0:
        print("✅ SUCCESS: Full pipeline worked!")
        print("Generated code:")
        print(stdout)
        return True
    else:
        print("❌ ERROR: Full pipeline failed!")
        print(f"Return code: {returncode}")
        print(f"stdout: {stdout}")
        print(f"stderr: {stderr}")
        return False


def main():
    """Main test function"""
    print("=" * 60)
    print("JVM IDL Plugin and Python311 Host Compiler Test")
    print("=" * 60)
    
    # Check if JAR files exist
    current_dir = os.path.dirname(os.path.abspath(__file__))
    jar_files = ["log4j-api-2.21.1.jar", "log4j-core-2.21.1.jar"]
    
    for jar_file in jar_files:
        jar_path = os.path.join(current_dir, jar_file)
        if not os.path.exists(jar_path):
            print(f"❌ ERROR: JAR file not found: {jar_path}")
            return 1
        else:
            print(f"✅ Found JAR file: {jar_file}")
    
    # Run tests
    test1_passed = test_idl_plugin_with_jars()
    test2_passed = test_host_compiler_with_idl()
    test3_passed = test_full_pipeline()
    
    print("\n" + "=" * 60)
    print("Test Results Summary:")
    print("=" * 60)
    print(f"IDL Plugin Test: {'✅ PASSED' if test1_passed else '❌ FAILED'}")
    print(f"Host Compiler Test: {'✅ PASSED' if test2_passed else '❌ FAILED'}")
    print(f"Full Pipeline Test: {'✅ PASSED' if test3_passed else '❌ FAILED'}")
    
    if test1_passed and test2_passed and test3_passed:
        print("\n🎉 ALL TESTS PASSED! The JVM IDL plugin and Python311 host compiler are working correctly.")
        return 0
    else:
        print("\n❌ SOME TESTS FAILED! Please check the JVM IDL plugin and Python311 host compiler implementations.")
        return 1


if __name__ == '__main__':
    exit(main()) 
