#pragma once
#include <runtime/cdts_wrapper.h>
#include <memory>
#include "python3_api_wrapper.h"

#include <string>
#include <utils/scope_guard.hpp>
#include <thread>

bool is_pycallable_or_property(PyObject* obj);
bool is_property(PyObject* obj);

// get current python error message
std::string check_python_error();

extern PyThreadState* _save;

#define pyscope() \
	PyGILState_STATE gstate = PyGILState_Ensure(); \
	metaffi::utils::scope_guard sggstate([&]() \
	{ \
		PyGILState_Release(gstate); \
	});
