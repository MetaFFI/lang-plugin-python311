#pragma once

#include <metaffi/api/metaffi_api.h>

#include <filesystem>
#include <string>

struct PythonTestEnv
{
	metaffi::api::MetaFFIRuntime runtime;
	metaffi::api::MetaFFIModule module_dir;
	metaffi::api::MetaFFIModule module_file;
	metaffi::api::MetaFFIModule builtins;
	metaffi::api::MetaFFIModule importlib;
	metaffi::api::MetaFFIModule asyncio;

	PythonTestEnv();
	~PythonTestEnv();
};

PythonTestEnv& python_test_env();
