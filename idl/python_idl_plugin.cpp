#include "../plugin-sdk/compiler/idl_plugin_interface.h"
#include "../plugin-sdk/runtime/xllr_capi_loader.h"
#include "../runtime/python3_api_wrapper.h"
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <array>

class PythonIDLPlugin : public idl_plugin_interface
{
private:
    bool python_initialized = false;

    void initialize_python()
    {
        if (!python_initialized) {
            if (!load_python3_api()) {
                throw std::runtime_error("Failed to load Python API");
            }
            pPy_Initialize();
            python_initialized = true;
        }
    }

    std::string extract_and_generate_idl(const std::string& file_path)
    {
        initialize_python();
        
        // Import required modules
        PyObject* sys = pPyImport_ImportModule("sys");
        PyObject* os = pPyImport_ImportModule("os");
        PyObject* ast = pPyImport_ImportModule("ast");
        PyObject* json = pPyImport_ImportModule("json");
        PyObject* inspect = pPyImport_ImportModule("inspect");
        
        if (!sys || !os || !ast || !json || !inspect) {
            return "{\"error\": \"Failed to import required Python modules\"}";
        }

        // Create the extraction code as a string
        std::string extractor_code = R"(
import ast
import json
import inspect
import sys
import os
from typing import Dict, List, Any, Optional

def safe_getattr(obj, attr, default=None):
    """Safely get attribute from object"""
    try:
        return getattr(obj, attr, default)
    except:
        return default

def extract_type_annotation(annotation):
    """Extract type annotation as string"""
    if annotation is None:
        return "Any"
    try:
        return ast.unparse(annotation) if hasattr(ast, 'unparse') else str(annotation)
    except:
        return str(annotation)

def extract_function_info(func_node, module_name=""):
    """Extract function information from AST node"""
    func_info = {
        "name": func_node.name,
        "type": "function",
        "parameters": [],
        "return_type": "Any",
        "docstring": ast.get_docstring(func_node) or ""
    }
    
    # Extract parameters
    for arg in func_node.args.args:
        param_info = {
            "name": arg.arg,
            "type": extract_type_annotation(arg.annotation)
        }
        func_info["parameters"].append(param_info)
    
    # Extract return type
    if func_node.returns:
        func_info["return_type"] = extract_type_annotation(func_node.returns)
    
    return func_info

def extract_class_info(class_node, module_name=""):
    """Extract class information from AST node"""
    class_info = {
        "name": class_node.name,
        "type": "class",
        "methods": [],
        "fields": [],
        "docstring": ast.get_docstring(class_node) or ""
    }
    
    # Extract methods and fields
    for item in class_node.body:
        if isinstance(item, ast.FunctionDef):
            method_info = extract_function_info(item, module_name)
            method_info["name"] = item.name
            class_info["methods"].append(method_info)
        elif isinstance(item, ast.Assign):
            # Class variables
            for target in item.targets:
                if isinstance(target, ast.Name):
                    field_info = {
                        "name": target.id,
                        "type": "Any"
                    }
                    class_info["fields"].append(field_info)
    
    return class_info

def extract_global_variables(module_node):
    """Extract global variables from module"""
    globals_list = []
    
    for item in module_node.body:
        if isinstance(item, ast.Assign):
            for target in item.targets:
                if isinstance(target, ast.Name):
                    global_info = {
                        "name": target.id,
                        "type": "Any"
                    }
                    globals_list.append(global_info)
    
    return globals_list

def extract_module_info(file_path):
    """Extract all information from a Python module"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            source_code = f.read()
        
        tree = ast.parse(source_code)
        
        module_info = {
            "name": os.path.splitext(os.path.basename(file_path))[0],
            "functions": [],
            "classes": [],
            "globals": []
        }
        
        # Extract all entities
        for item in tree.body:
            if isinstance(item, ast.FunctionDef):
                func_info = extract_function_info(item)
                module_info["functions"].append(func_info)
            elif isinstance(item, ast.ClassDef):
                class_info = extract_class_info(item)
                module_info["classes"].append(class_info)
            elif isinstance(item, ast.Assign):
                # Global variables
                for target in item.targets:
                    if isinstance(target, ast.Name):
                        global_info = {
                            "name": target.id,
                            "type": "Any"
                        }
                        module_info["globals"].append(global_info)
        
        return module_info
        
    except Exception as e:
        return {"error": f"Failed to extract module info: {str(e)}"}

def generate_metaffi_idl(module_info):
    """Generate MetaFFI IDL JSON from extracted module info"""
    if "error" in module_info:
        return json.dumps({"error": module_info["error"]})
    
    metaffi_idl = {
        "modules": [
            {
                "name": module_info["name"],
                "functions": [],
                "classes": []
            }
        ]
    }
    
    module = metaffi_idl["modules"][0]
    
    # Convert functions
    for func in module_info["functions"]:
        metaffi_func = {
            "name": func["name"],
            "parameters": func["parameters"],
            "return_type": func["return_type"]
        }
        module["functions"].append(metaffi_func)
    
    # Convert classes
    for cls in module_info["classes"]:
        metaffi_class = {
            "name": cls["name"],
            "methods": [],
            "fields": cls["fields"]
        }
        
        for method in cls["methods"]:
            metaffi_method = {
                "name": method["name"],
                "parameters": method["parameters"],
                "return_type": method["return_type"]
            }
            metaffi_class["methods"].append(metaffi_method)
        
        module["classes"].append(metaffi_class)
    
    return json.dumps(metaffi_idl, indent=2)

def process_file(file_path):
    """Process a single file and return the IDL JSON"""
    module_info = extract_module_info(file_path)
    return generate_metaffi_idl(module_info)
)";

        // Execute the embedded Python code
        PyObject* main_module = pPyImport_AddModule("__main__");
        PyObject* globals = pPyModule_GetDict(main_module);
        
        // Execute the extractor code
        int result = pPyRun_SimpleString(extractor_code.c_str());
        if (result != 0) {
            pPyErr_Print();
            return "{\"error\": \"Failed to execute Python extractor code\"}";
        }
        
        // Call the process_file function with file_path
        PyObject* process_func = pPyDict_GetItemString(globals, "process_file");
        if (!process_func) {
            return "{\"error\": \"process_file function not found\"}";
        }
        
        // Create arguments for process_file function
        PyObject* args = pPyTuple_New(1);
        pPyTuple_SetItem(args, 0, pPyUnicode_FromString(file_path.c_str()));
        PyObject* call_result = pPyObject_CallObject(process_func, args);
        
        if (!call_result) {
            pPyErr_Print();
            return "{\"error\": \"Failed to execute process_file function\"}";
        }
        
        // Get the result as string
        PyObject* str_result = pPyObject_Repr(call_result);
        const char* result_str = pPyUnicode_AsUTF8(str_result);
        std::string json_result = result_str ? result_str : "{\"error\": \"Failed to get result\"}";
        
        // Cleanup
        Py_DECREF(args);
        Py_DECREF(call_result);
        Py_DECREF(str_result);
        
        return json_result;
    }

public:
    void init() override
    {
        initialize_python();
        std::cout << "Python IDL Plugin initialized with embedded Python code." << std::endl;
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

            // Write source code to temporary file if needed
            if (!source.empty())
            {
                std::ofstream temp_file(file_path);
                temp_file.write(source_code, source_length);
                temp_file.close();
            }

            // Extract and generate IDL using embedded Python code
            std::string idl_json = extract_and_generate_idl(file_path);
            
            if (idl_json.find("\"error\"") != std::string::npos) {
                *out_err = xllr_alloc_string(idl_json.c_str(), idl_json.length());
                *out_err_len = idl_json.length();
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
        }
        catch (...)
        {
            *out_err = xllr_alloc_string("Unknown error in parse_idl", 26);
            *out_err_len = 26;
        }
    }

    ~PythonIDLPlugin()
    {
        if (python_initialized) {
            pPy_Finalize();
        }
    }
};

// Export the plugin instance
extern "C"
{
    // C wrapper for parse_idl
    void parse_idl(const char* source_code, uint32_t source_code_length,
                   const char* file_path, uint32_t file_path_length,
                   char** out_idl_def_json, uint32_t* out_idl_def_json_length,
                   char** out_err, uint32_t* out_err_len)
    {
        static PythonIDLPlugin plugin;
        static bool initialized = false;
        
        if (!initialized) {
            plugin.init();
            initialized = true;
        }
        
        plugin.parse_idl(source_code, source_code_length,
                        file_path, file_path_length,
                        out_idl_def_json, out_idl_def_json_length,
                        out_err, out_err_len);
    }
}
 