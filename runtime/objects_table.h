#pragma once
#include <utils/singleton.hpp>
#include <set>
#include <runtime/metaffi_primitives.h>
#include <shared_mutex>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

extern "C" void python3_release_object(metaffi_handle h);

class python3_objects_table_impl
{
private:
	std::set<PyObject*> objects;
	mutable std::shared_mutex m;

public:
	python3_objects_table_impl() = default;
	~python3_objects_table_impl() = default;
	
	void free();
	
	void set(PyObject* obj);
	void remove(PyObject* obj);
	bool contains(PyObject* obj) const;
	
	size_t size() const;
};

typedef metaffi::utils::singleton<python3_objects_table_impl> python3_objects_table;
template class metaffi::utils::singleton<python3_objects_table_impl>;