#pragma once
#include <runtime/cdts_wrapper.h>
#include<unordered_map>
#include <memory>
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <string>
#include <utils/scope_guard.hpp>
#include <thread>

// get current python error message
std::string get_py_error(void);

extern PyThreadState* _save;

#define pyscope() \
	PyGILState_STATE gstate = PyGILState_Ensure(); \
	metaffi::utils::scope_guard sggstate([&]() \
	{ \
		PyGILState_Release(gstate); \
	});
