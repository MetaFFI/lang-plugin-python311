#pragma once
#include <utils/singleton.hpp>
#include <Python.h>
#include <set>
#include <runtime/openffi_primitives.h>
#include <shared_mutex>

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
};

typedef openffi::utils::singleton<objects_table_impl> objects_table;
template class openffi::utils::singleton<objects_table_impl>;