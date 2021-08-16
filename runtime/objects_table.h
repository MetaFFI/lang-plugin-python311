#pragma once
#include <utils/singleton.hpp>
#include <Python.h>
#include <set>
#include <runtime/metaffi_primitives.h>
#include <shared_mutex>

extern "C" void release_object(metaffi_handle h);

class objects_table_impl
{
private:
	std::set<PyObject*> objects;
	mutable std::shared_mutex m;

public:
	objects_table_impl() = default;
	~objects_table_impl() = default;
	
	void free();
	
	void set(PyObject* obj);
	void remove(PyObject* obj);
	bool contains(PyObject* obj) const;
	
	size_t size() const;
};

typedef metaffi::utils::singleton<objects_table_impl> objects_table;
template class metaffi::utils::singleton<objects_table_impl>;