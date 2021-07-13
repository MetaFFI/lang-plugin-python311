#include <string>
#include <utils/scope_guard.hpp>

// get current python error message
std::string get_py_error(void);

#define pyscope()\
	PyGILState_STATE gstate = PyGILState_Ensure(); \
	openffi::utils::scope_guard sggstate([&]() \
	{ \
		PyGILState_Release(gstate); \
	});

