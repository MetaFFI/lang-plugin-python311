#!/usr/bin/env python3
"""
End-to-end test runner for OpenJDK -> Python311 MetaFFI compilation.

This script:
1. Runs the MetaFFI compiler to generate Python code from Java classes
2. Runs the end-to-end test using the generated code
"""

import os
import sys
import subprocess
import shlex
import unittest


def run_command(command: str, description: str = ""):
    """Run a command and handle errors"""
    print(f"\n{'='*60}")
    print(f"Running: {description or command}")
    print(f"Command: {command}")
    print(f"{'='*60}")
    
    try:
        command_split = shlex.split(os.path.expanduser(os.path.expandvars(command)))
        result = subprocess.run(command_split, capture_output=True, text=True)
        
        if result.stdout:
            print("STDOUT:")
            print(result.stdout)
        
        if result.stderr:
            print("STDERR:")
            print(result.stderr)
        
        if result.returncode != 0:
            print(f"❌ Command failed with exit code {result.returncode}")
            return False
        
        print("✅ Command completed successfully")
        return True
        
    except Exception as e:
        print(f"❌ Failed to run command: {e}")
        return False


def check_generated_files():
    """Check if the generated Python files exist"""
    expected_files = [
        'sanity.py',
        'sanity/__init__.py',
        'sanity/TestRuntime.py',
        'sanity/TestMap.py'
    ]
    
    missing_files = []
    for file_path in expected_files:
        if not os.path.exists(file_path):
            missing_files.append(file_path)
    
    if missing_files:
        print(f"❌ Missing generated files: {missing_files}")
        return False
    
    print("✅ All expected generated files found")
    return True


def main():
    """Main function to run the end-to-end test"""
    print("🚀 Starting OpenJDK -> Python311 MetaFFI End-to-End Test")
    print(f"Working directory: {os.getcwd()}")
    
    # Step 1: Build the MetaFFI code
    print("\n📦 Step 1: Building MetaFFI code...")
    if not run_command("python build_metaffi.py", "Building MetaFFI code from Java classes"):
        print("❌ Failed to build MetaFFI code. Exiting.")
        return 1
    
    # Step 2: Check if generated files exist
    print("\n🔍 Step 2: Checking generated files...")
    if not check_generated_files():
        print("❌ Generated files not found. Exiting.")
        return 1
    
    # Step 3: Run the end-to-end test
    print("\n🧪 Step 3: Running end-to-end test...")
    if not run_command("python openjdk_test_with_compiler.py", "Running end-to-end test with generated code"):
        print("❌ End-to-end test failed. Exiting.")
        return 1
    
    print("\n🎉 All tests completed successfully!")
    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code) 