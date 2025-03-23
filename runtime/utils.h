#pragma once
#include <runtime/cdts_wrapper.h>
#include <memory>
#include "python3_api_wrapper.h"

#include <string>
#include <utils/scope_guard.hpp>
#include <thread>

// get current python error message
std::string check_python_error();

extern PyThreadState* _save;

#define pyscope() \
	PyGILState_STATE gstate = pPyGILState_Ensure(); \
	metaffi::utils::scope_guard sggstate([&]() \
	{ \
		pPyGILState_Release(gstate); \
	});
