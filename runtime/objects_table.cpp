#include "objects_table.h"

//--------------------------------------------------------------------
void objects_table_impl::free()
{
	std::unique_lock l(m);
	
	for(auto it : this->objects)
	{
		Py_DecRef(it);
	}
}
//--------------------------------------------------------------------
void objects_table_impl::set(PyObject* obj)
{
	std::unique_lock l(m); // TODO need to use upgradable lock!
	
	auto it = this->objects.find(obj);
	if(it != this->objects.end()){
		return;
	}
	
	Py_IncRef(obj);
	this->objects.insert(obj);
}
//--------------------------------------------------------------------
void objects_table_impl::remove(PyObject* obj)
{
	std::unique_lock l(m);
	
	auto it = this->objects.find(obj);
	if(it == this->objects.end()){
		return;
	}
	
	Py_DecRef(obj);
	this->objects.erase(obj);
}
//--------------------------------------------------------------------
bool objects_table_impl::contains(PyObject* obj) const
{
	std::shared_lock l(m);
	
	return this->objects.find(obj) != this->objects.end();
}
//--------------------------------------------------------------------
size_t objects_table_impl::size() const
{
	std::shared_lock l(m);
	return this->objects.size();
}
//--------------------------------------------------------------------
void release_object(metaffi_handle h)
{
	objects_table::instance().remove((PyObject*)h);
}
//--------------------------------------------------------------------