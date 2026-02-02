#pragma once

#include <runtime_manager/cpython3/runtime_manager.h>
#include <memory>
#include <string>

/**
 * Python3 Compiler Plugin
 *
 * Compiles MetaFFI IDL to Python host code.
 * All methods use C++ types and may throw exceptions.
 * C-compatible API is provided separately in compiler_api.cpp.
 */
class PythonCompilerPlugin
{
private:
    std::shared_ptr<cpython3_runtime_manager> m_runtime;

    void initialize_python();

    void execute_host_compiler(
        const std::string& idl_def_json,
        const std::string& output_path,
        const std::string& host_options);

public:
    PythonCompilerPlugin() = default;
    ~PythonCompilerPlugin();

    // Non-copyable, non-movable (RAII with Python runtime)
    PythonCompilerPlugin(const PythonCompilerPlugin&) = delete;
    PythonCompilerPlugin& operator=(const PythonCompilerPlugin&) = delete;
    PythonCompilerPlugin(PythonCompilerPlugin&&) = delete;
    PythonCompilerPlugin& operator=(PythonCompilerPlugin&&) = delete;

    /**
     * Initialize the plugin (loads Python runtime).
     * @throws std::runtime_error if Python cannot be loaded
     */
    void init();

    /**
     * Compile IDL to guest code.
     * @throws std::runtime_error always - Python3 does not support guest compilation
     */
    void compile_to_guest(
        const std::string& idl_def_json,
        const std::string& output_path,
        const std::string& guest_options);

    /**
     * Compile IDL to host code.
     * @param idl_def_json IDL definition as JSON string
     * @param output_path Output path for generated files
     * @param host_options Compiler options as JSON string (can be empty)
     * @throws std::runtime_error on compilation failure
     */
    void compile_from_host(
        const std::string& idl_def_json,
        const std::string& output_path,
        const std::string& host_options);
};
