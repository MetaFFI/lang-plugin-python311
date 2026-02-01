#include <compiler/idl_plugin_interface.h>
#include <runtime/xllr_capi_loader.h>
#include <runtime_manager/cpython3/runtime_manager.h>
#include <runtime_manager/cpython3/python_api_wrapper.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <utils/safe_func.h>

class PythonIDLPlugin : public idl_plugin_interface
{
private:
    std::shared_ptr<cpython3_runtime_manager> m_runtime;

    void initialize_python()
    {
        if (m_runtime)
        {
            return;
        }

        // Auto-detect Python version
        auto versions = cpython3_runtime_manager::detect_installed_python3();
        if (versions.empty())
        {
            throw std::runtime_error("No Python 3.x installation found");
        }

        // Use latest available version (last in list)
        m_runtime = cpython3_runtime_manager::create(versions.back());

        // Add SDK path to sys.path for importing sdk.idl_compiler.python3.*
        // SDK is expected to be in METAFFI_HOME or current working directory
        char* metaffi_home = metaffi_getenv_alloc("METAFFI_HOME");
        if (metaffi_home)
        {
            std::filesystem::path sdk_path = std::filesystem::path(metaffi_home);
            m_runtime->add_sys_path(sdk_path.string());
            metaffi_free_env(metaffi_home);
        }
        else
        {
            // Fallback to current directory
            std::filesystem::path sdk_path = std::filesystem::current_path();
            m_runtime->add_sys_path(sdk_path.string());
        }
    }

    std::string extract_and_generate_idl(const std::string& file_path)
    {
        initialize_python();

        // Acquire GIL for Python operations
        auto gil = m_runtime->acquire_gil();

        // Import the SDK IDL compiler modules
        PyObject* extractor_module = pPyImport_ImportModule("sdk.idl_compiler.python3.extractor");
        if (!extractor_module)
        {
            pPyErr_Print();
            return "{\"error\": \"Failed to import sdk.idl_compiler.python3.extractor\"}";
        }

        PyObject* generator_module = pPyImport_ImportModule("sdk.idl_compiler.python3.idl_generator");
        if (!generator_module)
        {
            pPyErr_Print();
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to import sdk.idl_compiler.python3.idl_generator\"}";
        }

        // Get PythonExtractor class
        PyObject* extractor_class = pPyObject_GetAttrString(extractor_module, "PythonExtractor");
        if (!extractor_class)
        {
            pPyErr_Print();
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to get PythonExtractor class\"}";
        }

        // Create extractor instance with file path
        PyObject* extractor_args = pPyTuple_New(1);
        pPyTuple_SetItem(extractor_args, 0, pPyUnicode_FromString(file_path.c_str()));
        PyObject* extractor = pPyObject_CallObject(extractor_class, extractor_args);
        Py_DECREF(extractor_args);
        Py_DECREF(extractor_class);

        if (!extractor)
        {
            pPyErr_Print();
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to create PythonExtractor instance\"}";
        }

        // Call extract() method
        PyObject* extract_method = pPyObject_GetAttrString(extractor, "extract");
        if (!extract_method)
        {
            pPyErr_Print();
            Py_DECREF(extractor);
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to get extract method\"}";
        }

        PyObject* module_info = pPyObject_CallObject(extract_method, nullptr);
        Py_DECREF(extract_method);

        if (!module_info)
        {
            pPyErr_Print();
            Py_DECREF(extractor);
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to extract module info\"}";
        }

        // Get IDLGenerator class
        PyObject* generator_class = pPyObject_GetAttrString(generator_module, "IDLGenerator");
        if (!generator_class)
        {
            pPyErr_Print();
            Py_DECREF(module_info);
            Py_DECREF(extractor);
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to get IDLGenerator class\"}";
        }

        // Create generator instance with file_path and module_info
        PyObject* generator_args = pPyTuple_New(2);
        pPyTuple_SetItem(generator_args, 0, pPyUnicode_FromString(file_path.c_str()));
        Py_INCREF(module_info);  // PyTuple_SetItem steals reference
        pPyTuple_SetItem(generator_args, 1, module_info);
        PyObject* generator = pPyObject_CallObject(generator_class, generator_args);
        Py_DECREF(generator_args);
        Py_DECREF(generator_class);

        if (!generator)
        {
            pPyErr_Print();
            Py_DECREF(module_info);
            Py_DECREF(extractor);
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to create IDLGenerator instance\"}";
        }

        // Call generate_json() method
        PyObject* generate_json_method = pPyObject_GetAttrString(generator, "generate_json");
        if (!generate_json_method)
        {
            pPyErr_Print();
            Py_DECREF(generator);
            Py_DECREF(module_info);
            Py_DECREF(extractor);
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to get generate_json method\"}";
        }

        PyObject* json_result = pPyObject_CallObject(generate_json_method, nullptr);
        Py_DECREF(generate_json_method);

        if (!json_result)
        {
            pPyErr_Print();
            Py_DECREF(generator);
            Py_DECREF(module_info);
            Py_DECREF(extractor);
            Py_DECREF(generator_module);
            Py_DECREF(extractor_module);
            return "{\"error\": \"Failed to generate JSON\"}";
        }

        // Convert result to C++ string
        const char* json_str = pPyUnicode_AsUTF8(json_result);
        std::string result = json_str ? json_str : "{\"error\": \"Failed to convert JSON to string\"}";

        // Cleanup
        Py_DECREF(json_result);
        Py_DECREF(generator);
        Py_DECREF(module_info);
        Py_DECREF(extractor);
        Py_DECREF(generator_module);
        Py_DECREF(extractor_module);

        return result;
    }

public:
    void init() override
    {
        initialize_python();
        std::cout << "Python IDL Plugin initialized (using SDK cpython3_runtime_manager)" << std::endl;
    }

    void parse_idl(const char* source_code, uint32_t source_length,
                   const char* file_or_path, uint32_t file_or_path_length,
                   char** out_idl_def_json, uint32_t* out_idl_def_json_length,
                   char** out_err, uint32_t* out_err_len) override
    {
        try
        {
            // Convert input to strings
            std::string file_path(file_or_path, file_or_path_length);
            std::string source(source_code, source_length);

            // Write source code to temporary file if provided
            if (!source.empty())
            {
                std::ofstream temp_file(file_path);
                temp_file.write(source_code, source_length);
                temp_file.close();
            }

            // Extract and generate IDL using SDK
            std::string idl_json = extract_and_generate_idl(file_path);

            // Check for errors in JSON result
            if (idl_json.find("\"error\"") != std::string::npos)
            {
                *out_err = xllr_alloc_string(idl_json.c_str(), idl_json.length());
                *out_err_len = idl_json.length();
                *out_idl_def_json = nullptr;
                *out_idl_def_json_length = 0;
                return;
            }

            // Return the generated JSON
            *out_idl_def_json = xllr_alloc_string(idl_json.c_str(), idl_json.length());
            *out_idl_def_json_length = idl_json.length();
            *out_err = nullptr;
            *out_err_len = 0;
        }
        catch (const std::exception& e)
        {
            *out_err = xllr_alloc_string(e.what(), strlen(e.what()));
            *out_err_len = strlen(e.what());
            *out_idl_def_json = nullptr;
            *out_idl_def_json_length = 0;
        }
        catch (...)
        {
            const char* error_msg = "Unknown error in parse_idl";
            *out_err = xllr_alloc_string(error_msg, strlen(error_msg));
            *out_err_len = strlen(error_msg);
            *out_idl_def_json = nullptr;
            *out_idl_def_json_length = 0;
        }
    }

    ~PythonIDLPlugin()
    {
        // Runtime manager handles cleanup automatically via RAII
        m_runtime.reset();
    }
};

// Export the plugin entry point
extern "C"
{
    // C wrapper for parse_idl - maintains backward compatibility
    void parse_idl(const char* source_code, uint32_t source_code_length,
                   const char* file_path, uint32_t file_path_length,
                   char** out_idl_def_json, uint32_t* out_idl_def_json_length,
                   char** out_err, uint32_t* out_err_len)
    {
        static PythonIDLPlugin plugin;
        static bool initialized = false;

        if (!initialized)
        {
            plugin.init();
            initialized = true;
        }

        plugin.parse_idl(source_code, source_code_length,
                        file_path, file_path_length,
                        out_idl_def_json, out_idl_def_json_length,
                        out_err, out_err_len);
    }
}
