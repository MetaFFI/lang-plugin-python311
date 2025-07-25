# MetaFFI IDL Plugin Development Knowledge Base

## Overview
IDL (Interface Definition Language) plugins in MetaFFI are responsible for extracting language-specific entities (functions, classes, globals) from source code and converting them into MetaFFI-compliant JSON format. This document covers the general patterns and architecture for building IDL plugins.

## Plugin SDK Architecture

### Core Interfaces
- **`idl_plugin_interface.h`**: Defines the C++ interface that all IDL plugins must implement
  - `init()`: Initialize the plugin
  - `parse_idl()`: Main function that extracts entities and returns JSON IDL definition
- **`language_plugin_interface.h`**: Defines interfaces for language-specific plugins (compiler, runtime)

### Plugin SDK Structure
```
plugin-sdk/
├── compiler/
│   ├── idl_plugin_interface.h      # IDL plugin interface
│   ├── language_plugin_interface.h  # Language plugin interface
│   └── go/                         # Go implementation helpers
│       ├── IDL/                    # MetaFFI IDL structs
│       ├── IDLPluginInterfaceHandler.go
│       └── LanguagePluginInterfaceHandler.go
├── runtime/                        # Runtime plugin interfaces
└── utils/                          # Shared utilities
```

## IDL Plugin Implementation Patterns

### 1. Dynamic Library Requirements
- Must be built as a dynamic library (`.so` on Linux, `.dll` on Windows)
- Must export the functions defined in `idl_plugin_interface.h`
- Naming convention: `metaffi.idl.{language}.{extension}` (e.g., `metaffi.idl.python311.so`)

### 2. Language Integration Approaches

#### Option A: C++ with Embedded Language Runtime (Recommended)
- **Pattern**: C++ dynamic library that embeds the target language interpreter
- **Pros**: No external dependencies, self-contained, follows MetaFFI patterns
- **Cons**: More complex, requires language-specific embedding
- **Example**: Python plugin using `load_python3_api()` with function pointers

#### Option B: C++ Wrapper Around Language Process
- **Pattern**: C++ dynamic library that spawns language processes
- **Pros**: Simpler C++ code, leverages existing language tools
- **Cons**: Process overhead, more complex IPC

#### Option C: Pure Language Implementation with C Interface
- **Pattern**: Language-specific implementation with C FFI bindings
- **Pros**: Leverages language strengths for parsing
- **Cons**: Requires language runtime, more dependencies

### 3. XLLR Integration
- **Purpose**: XLLR (Cross-Language Runtime) provides memory allocation and cross-language communication
- **Loading**: Use `xllr_static_capi_loader.cpp` for automatic XLLR loading
- **Memory Management**: Use `xllr_alloc_string()` for error messages and string allocation
- **Error Handling**: All errors must be allocated using XLLR functions, not standard C allocation

### 4. Language Runtime Loading Patterns

#### Dynamic Loading (Recommended)
- **Pattern**: Load language runtime dynamically at runtime
- **Benefits**: No compile-time dependencies, version flexibility
- **Implementation**: Use `dlopen`/`LoadLibrary` with function pointers
- **Example**: Python plugin loads `libpython3.11.so` dynamically

#### Static Linking (Not Recommended)
- **Pattern**: Link directly to language runtime
- **Drawbacks**: Version lock-in, deployment complexity
- **Avoid**: Creates dependencies on specific language versions

### 5. Entity Extraction Patterns

#### File-based Extraction
- **Input**: Path to source file
- **Process**: Parse single file, extract entities
- **Output**: Single module with entities

#### Package/Module Extraction
- **Input**: Package name or module path
- **Process**: Import module, extract all entities recursively
- **Output**: Single module containing all package entities

#### Directory-based Extraction
- **Input**: Directory path
- **Process**: Scan directory for source files, extract from each
- **Output**: Single module with all entities

### 6. JSON Output Structure
- **Schema**: Must conform to MetaFFI IDL JSON schema
- **Structure**: Single module containing all entities
- **Entities**: Functions, classes, globals, constants
- **Metadata**: Comments, type information, entity paths

## Build System Integration

### CMake Structure
```
plugin/
├── CMakeLists.txt              # Main plugin CMakeLists
├── idl/
│   ├── CMakeLists.txt          # IDL-specific build
│   ├── {language}_extractor    # Language-specific extraction
│   └── {language}_idl_generator # JSON generation
├── runtime/
│   └── CMakeLists.txt          # Runtime plugin build
└── plugin-sdk/                 # Plugin SDK (submodule)
```

### Build Patterns
- **Independent Build**: IDL plugin can be built independently
- **Shared Dependencies**: Can link to existing runtime libraries
- **Testing**: Separate unit tests for extraction logic and C interface

### Output Directory Structure
- **Plugin Output**: Build to `./{language}` directory (e.g., `./python311`)
- **Test Naming**: Use `{language}_idl_plugin_test` for test executables
- **Dependencies**: Add test dependencies on the plugin library

## Testing Strategies

### Unit Testing
- **Language Logic**: Test entity extraction in target language
- **JSON Generation**: Test conversion to MetaFFI JSON format
- **Error Handling**: Test various error conditions

### Integration Testing
- **C Interface**: Test `parse_idl()` function with doctest
- **Dynamic Library**: Test loading and function calls
- **End-to-End**: Test complete extraction pipeline

### Test Scenarios
1. **File Parsing**: Single source file extraction
2. **Package Parsing**: Python package/module extraction
3. **Error Conditions**: Invalid files, missing dependencies
4. **Edge Cases**: Complex types, nested structures

## Common Patterns and Best Practices

### 1. Error Handling
```cpp
// Correct pattern
*out_err = xllr_alloc_string(error_message.c_str(), error_message.length());

// Wrong pattern
*out_err = strdup(error_message.c_str());
```

### 2. Memory Management
- Use XLLR allocation functions for strings
- Follow language-specific reference counting (e.g., Py_INCREF/Py_DECREF)
- Clean up resources in error paths

### 3. Thread Safety
- Acquire language runtime locks (e.g., Python GIL)
- Use RAII patterns for resource management
- Handle concurrent access to shared resources

### 4. Platform Compatibility
- Handle different library names (e.g., `libpython3.11.so` vs `python311.dll`)
- Support both static and dynamic loading patterns
- Handle different symbol naming conventions

## Language-Specific Considerations

### Python Plugins
- **Runtime Loading**: Dynamic loading of Python interpreter
- **GIL Management**: Acquire/release Python Global Interpreter Lock
- **Module Import**: Use `importlib.import_module()` for package extraction
- **Type Information**: Extract from `__annotations__` and function signatures
- **Embedded Code**: Embed Python extraction logic as string in C++

### Go Plugins
- **Build System**: Use Go's build system with CGO
- **Symbol Export**: Export C functions for dynamic library interface
- **Memory Management**: Go's garbage collector handles most memory

### Java Plugins
- **JVM Integration**: Embed JVM or use JNI
- **Class Loading**: Use reflection for entity extraction
- **Type System**: Leverage Java's rich type system

## Deployment Considerations

### Dependencies
- **Minimal Dependencies**: Avoid linking to language runtimes
- **Dynamic Loading**: Load language runtimes at runtime
- **Version Flexibility**: Support multiple language versions

### Distribution
- **Self-Contained**: Include necessary language runtimes
- **Platform Support**: Build for target platforms
- **Installation**: Follow platform-specific installation patterns

## Debugging and Development

### Common Issues
1. **Symbol Loading**: Missing function pointers from dynamic libraries
2. **Memory Leaks**: Incorrect reference counting in language runtimes
3. **Thread Issues**: Improper GIL/lock management
4. **Type Conversion**: Errors in converting between language types and MetaFFI types

### Debugging Tools
- **Dynamic Library Inspection**: `nm`, `objdump` for symbol inspection
- **Runtime Debugging**: Language-specific debuggers
- **Memory Profiling**: Valgrind, language-specific profilers

## Future Considerations

### Plugin SDK Evolution
- **Standardization**: Common patterns across all language plugins
- **Testing Framework**: Standardized testing approaches
- **Documentation**: Comprehensive plugin development guides

### Performance Optimization
- **Caching**: Cache extracted entities for repeated access
- **Parallel Processing**: Extract entities in parallel where possible
- **Incremental Updates**: Support incremental entity extraction

### Extensibility
- **Plugin Architecture**: Support for custom extraction logic
- **Configuration**: Runtime configuration of extraction behavior
- **Plugins**: Support for language-specific plugins within IDL plugins

---

## Python IDL Plugin: Extraction and Testing Insights

- The extractor (`py_extractor.py`) now uses `importlib.util.spec_from_file_location` for robust file-based imports, ensuring correct loading of modules from arbitrary file paths.
- The extractor **now extracts class variables** (static/class-level fields) by examining the class's `__dict__` for attributes that are not functions, methods, or properties. Class variables are included in the `fields` list for each class.
- Functions without explicit return type annotations are extracted with a single return value of type `"any"`.
- For package extraction tests, the temporary directory containing the package must be added to `sys.path` before importing, and removed after, to allow absolute imports to work.
- For complex type annotation tests, always ensure that all required types (e.g., `Any`, `Dict`, `List`) are imported in the generated test code.
- When counting class methods in tests, filter out dunder (methods starting with `__`) and built-in methods to only count user-defined methods.
- All Python unit tests must pass before proceeding to C++ integration.
- Remove obsolete files (e.g., `py_extractor_fixed.py`) once their changes are merged and all references are updated.
- Keep the extractor and generator logic in sync with the MetaFFI JSON schema and test expectations.
- Use comprehensive unit tests to catch edge cases in extraction and JSON generation.
- The generator now includes class fields in the JSON output under the `"fields"` key for each class, converting Python types to MetaFFI types using `py_type_to_metaffi_type()`.
- Class variables are properly extracted and included in the generated JSON, making them available for cross-language access through MetaFFI.
- The Go IDL parser can successfully parse and validate the generated JSON, confirming compatibility with the MetaFFI IDL schema.

## IDL Plugin Implementation Learnings

### Dynamic Loading Pattern
- **Always use dynamic loading** for language runtimes - never static link to Python.h or similar
- **Follow the runtime pattern**: Use function pointers loaded from dynamic libraries
- **Example**: `load_python3_api()` loads Python symbols at runtime, not compile time
- **Benefits**: No compile-time dependencies, version flexibility, self-contained deployment

### Embedded Code Strategy
- **Embed language-specific logic as strings** in C++ code for self-contained plugins
- **Execute via language C API**: Use `pPyRun_SimpleString()` and `pPyObject_CallObject()`
- **Avoid external file dependencies**: Keep all extraction logic embedded in the plugin
- **Benefits**: Single binary, no external dependencies, easier deployment

### Build System Integration
- **Output to language-specific directory**: Use `./{language}` (e.g., `./python311`)
- **Test naming convention**: Use `{language}_idl_plugin_test` for test executables
- **Add test dependencies**: `add_dependencies(python311_idl_plugin_test metaffi.idl.python311)`
- **Include runtime sources**: Add runtime wrapper files to build (e.g., `python3_api_wrapper.cpp`)

### Memory Management
- **Use XLLR allocation**: Always use `xllr_alloc_string()` for error messages
- **Follow language reference counting**: Use `Py_DECREF()` not `pPy_DECREF()`
- **Clean up in error paths**: Ensure resources are freed even when errors occur
- **RAII patterns**: Use smart pointers and RAII for resource management

### Error Handling Patterns
- **Return structured errors**: Use JSON error format for consistency
- **XLLR string allocation**: All error strings must use XLLR allocation
- **Graceful degradation**: Handle missing language runtimes gracefully
- **Detailed error messages**: Include context in error messages for debugging

### Testing Strategy
- **Unit tests for extraction logic**: Test language-specific extraction separately
- **Integration tests for C interface**: Use doctest for C interface validation
- **End-to-end tests**: Test complete pipeline from source to JSON
- **Error condition tests**: Test various failure scenarios

### Performance Considerations
- **Lazy initialization**: Initialize language runtime only when needed
- **Caching**: Consider caching extracted entities for repeated access
- **Memory efficiency**: Minimize object creation and copying
- **Thread safety**: Handle concurrent access to shared resources

### Deployment Best Practices
- **Self-contained**: Include all necessary runtime dependencies
- **Version compatibility**: Support multiple language versions
- **Platform independence**: Handle different library names and paths
- **Installation simplicity**: Follow platform-specific installation patterns

### Debugging Techniques
- **Symbol inspection**: Use `nm` and `objdump` to verify dynamic loading
- **Runtime debugging**: Use language-specific debuggers for embedded code
- **Memory profiling**: Use tools like Valgrind for memory leak detection
- **Error logging**: Implement comprehensive error logging for troubleshooting

### Future Extensibility
- **Plugin architecture**: Design for easy addition of new extraction features
- **Configuration support**: Allow runtime configuration of extraction behavior
- **Custom extraction logic**: Support for language-specific extraction plugins
- **Performance optimization**: Consider parallel processing and incremental updates
