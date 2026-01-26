#pragma once

#include <memory>
#include <runtime_manager/cpython3/runtime_manager.h>

// Global runtime manager - defined in python_api.cpp
extern std::shared_ptr<cpython3_runtime_manager> g_runtime_manager;

// Get the global runtime manager (throws if not initialized)
inline cpython3_runtime_manager& get_runtime_manager()
{
	if(!g_runtime_manager)
	{
		throw std::runtime_error("Python runtime manager not initialized");
	}
	return *g_runtime_manager;
}
