#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include <compiler/idl_plugin_interface.h>
#include <string>
#include <iostream>

// C interface declaration
extern "C" {
    void parse_idl(const char* source_code, uint32_t source_code_length,
                   const char* file_path, uint32_t file_path_length,
                   char** out_idl_def_json, uint32_t* out_idl_def_json_length,
                   char** out_err, uint32_t* out_err_len);
}

// Test the IDL plugin C interface
TEST_CASE("IDL Plugin C Interface Test")
{
    // Test data
    std::string test_code = R"(
class TestClass:
    """A test class with class variables"""
    
    # Class variables
    CLASS_CONSTANT = 42
    class_string = "hello"
    
    def __init__(self, name: str):
        self.name = name  # Instance variable
    
    def get_name(self) -> str:
        return self.name
    
    def set_name(self, name: str) -> None:
        self.name = name

def simple_function(x: int, y: str) -> bool:
    """A simple function with type annotations"""
    return True

def function_with_defaults(a: int = 1, b: str = "default") -> None:
    """Function with default values"""
    pass

# Global variables
MY_CONSTANT = 42
my_string = "hello"
)";

    std::string file_path = "test_file.py";

    // Call C interface parse_idl
    char* out_idl_def_json = nullptr;
    uint32_t out_idl_def_json_length = 0;
    char* out_err = nullptr;
    uint32_t out_err_len = 0;

    parse_idl(
        test_code.c_str(), test_code.length(),
        file_path.c_str(), file_path.length(),
        &out_idl_def_json, &out_idl_def_json_length,
        &out_err, &out_err_len);

    // Check for errors
    if (out_err != nullptr)
    {
        std::string error_msg(out_err, out_err_len);
        std::cerr << "Error: " << error_msg << std::endl;
        FAIL("IDL plugin returned an error");
    }

    // Check that we got JSON output
    REQUIRE(out_idl_def_json != nullptr);
    REQUIRE(out_idl_def_json_length > 0);

    // Convert to string for inspection
    std::string json_output(out_idl_def_json, out_idl_def_json_length);
    std::cout << "Generated JSON:" << std::endl << json_output << std::endl;

    // Basic validation of JSON structure
    CHECK(json_output.find("\"modules\"") != std::string::npos);
    CHECK(json_output.find("\"TestClass\"") != std::string::npos);
    CHECK(json_output.find("\"simple_function\"") != std::string::npos);
    CHECK(json_output.find("\"CLASS_CONSTANT\"") != std::string::npos);
}

// Test error handling
TEST_CASE("IDL Plugin Error Handling Test")
{
    // Test with invalid Python code
    std::string invalid_code = "def invalid syntax {";
    std::string file_path = "invalid_file.py";

    char* out_idl_def_json = nullptr;
    uint32_t out_idl_def_json_length = 0;
    char* out_err = nullptr;
    uint32_t out_err_len = 0;

    parse_idl(
        invalid_code.c_str(), invalid_code.length(),
        file_path.c_str(), file_path.length(),
        &out_idl_def_json, &out_idl_def_json_length,
        &out_err, &out_err_len);

    // Should get an error for invalid syntax
    CHECK(out_err != nullptr);
    CHECK(out_err_len > 0);
} 
