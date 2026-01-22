# JVM End-to-End Tests

This directory contains end-to-end tests for the JVM MetaFFI compiler plugin, demonstrating the complete flow from Java classes to generated Python code.

## Test Files

### Core Test Files
- **`jvm_test.py`** - Direct MetaFFI API test (without compiler)
- **`jvm_test_with_compiler.py`** - End-to-end test using generated Python code
- **`build_metaffi.py`** - Build script that runs the MetaFFI compiler
- **`run_e2e_test.py`** - Complete test runner (build + test)

### Java Source Files
- **`TestRuntime.java`** - Java class with various test methods
- **`TestMap.java`** - Java class with map-like functionality
- **`sanity/`** - Directory containing compiled `.class` files

## Test Flow

### 1. Direct API Test (`jvm_test.py`)
```
Python → MetaFFI API → Java Classes
```
- Tests the MetaFFI runtime directly
- Uses low-level API calls
- Validates basic functionality

### 2. End-to-End Test (`jvm_test_with_compiler.py`)
```
Java Classes → JVM IDL Plugin → MetaFFI IDL → Python311 Host Compiler → Generated Python Code → Test
```
- Tests the complete MetaFFI compiler pipeline
- Uses generated high-level Python interface
- Validates the full compilation process

## Running the Tests

### Quick Start
```bash
# Run the complete end-to-end test
python run_e2e_test.py
```

### Step by Step
```bash
# 1. Build the MetaFFI code
python build_metaffi.py

# 2. Run the direct API test
python jvm_test.py

# 3. Run the end-to-end test
python jvm_test_with_compiler.py
```

## Expected Generated Files

After running `build_metaffi.py`, you should see:
- `sanity.py` - Main module file
- `sanity/__init__.py` - Package initialization
- `sanity/TestRuntime.py` - Generated TestRuntime class
- `sanity/TestMap.py` - Generated TestMap class

## Test Coverage

### TestRuntime Methods
- `hello_world()` - Simple void method
- `returns_an_error()` - Method that throws an exception
- `div_integers(a, b)` - Method with parameters and return value
- `join_strings(strings)` - Method with array parameter
- `wait_a_bit(seconds)` - Method with integer parameter
- `five_seconds` - Property getter
- `call_callback(callback)` - Method with callback parameter

### TestMap Methods
- `__init__()` - Constructor
- `set(key, value)` - Set a key-value pair
- `get(key)` - Get a value by key
- `contains(key)` - Check if key exists
- `name` - Property getter/setter

## Troubleshooting

### Common Issues

1. **Generated files not found**
   - Run `python build_metaffi.py` first
   - Check that the MetaFFI CLI is installed and working

2. **Import errors in generated code**
   - Verify that the Python311 host compiler is properly installed
   - Check that all required MetaFFI dependencies are available

3. **Java class loading errors**
   - Ensure the `sanity/` directory contains valid `.class` files
   - Verify that the Java classes are compatible with the JVM IDL plugin

### Debug Mode
```bash
# Run with verbose output
python -v run_e2e_test.py

# Run individual tests with more detail
python -m unittest -v jvm_test_with_compiler.TestSanityWithCompiler
```

## Architecture Notes

### MetaFFI Compiler Pipeline
1. **JVM IDL Plugin**: Analyzes Java bytecode and generates MetaFFI IDL
2. **Python311 Host Compiler**: Takes MetaFFI IDL and generates Python code
3. **Generated Python Code**: Provides high-level interface to Java classes

### Key Differences Between Tests
- **Direct Test**: Uses `metaffi.metaffi_runtime.MetaFFIRuntime` directly
- **End-to-End Test**: Uses generated Python classes that wrap the MetaFFI API

## Contributing

When adding new tests:
1. Add Java methods to `TestRuntime.java` or `TestMap.java`
2. Compile the Java files to `.class` files in `sanity/`
3. Add corresponding test methods to both test files
4. Update this README if needed

## Dependencies

- MetaFFI CLI (`metaffi` command)
- JVM IDL Plugin
- Python311 Host Compiler
- Python 3.11+
- Java Runtime Environment (JRE) 