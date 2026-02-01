#include "python3_wrappers.h"

#include <cstring>

PyHandle::PyHandle(cdt_metaffi_handle* handle)
	: _handle(handle)
{
}

PyHandle::~PyHandle()
{
	if(_handle)
	{
		if(_handle->release)
		{
			_handle->release(_handle);
		}
		xllr_free_memory(_handle);
		_handle = nullptr;
	}
}

PyHandle::PyHandle(PyHandle&& other) noexcept
	: _handle(other._handle)
{
	other._handle = nullptr;
}

PyHandle& PyHandle::operator=(PyHandle&& other) noexcept
{
	if(this != &other)
	{
		if(_handle)
		{
			if(_handle->release)
			{
				_handle->release(_handle);
			}
			xllr_free_memory(_handle);
		}
		_handle = other._handle;
		other._handle = nullptr;
	}
	return *this;
}

cdt_metaffi_handle* PyHandle::get() const
{
	return _handle;
}

bool PyHandle::is_null() const
{
	return !_handle || !_handle->handle;
}

cdt_metaffi_handle* PyHandle::release()
{
	cdt_metaffi_handle* released = _handle;
	_handle = nullptr;
	return released;
}

std::string take_string8(metaffi_string8 value)
{
	if(!value)
	{
		return {};
	}
	std::string result(reinterpret_cast<const char*>(value));
	xllr_free_string(reinterpret_cast<char*>(value));
	return result;
}

PyList PyList::create()
{
	auto& env = python_test_env();
	auto ctor = env.builtins.load_entity("callable=list", {}, {metaffi_handle_type});
	auto [handle] = ctor.call<cdt_metaffi_handle*>();
	return PyList(PyHandle(handle));
}

PyList::PyList(PyHandle handle)
	: _handle(std::move(handle))
{
}

cdt_metaffi_handle* PyList::handle() const
{
	return _handle.get();
}

metaffi_size PyList::size() const
{
	auto& env = python_test_env();
	auto len_entity = env.builtins.load_entity("callable=len", {metaffi_handle_type}, {metaffi_int64_type});
	auto [size] = len_entity.call<int64_t>(*_handle.get());
	return static_cast<metaffi_size>(size);
}

PyHandle PyList::get_handle(metaffi_size index) const
{
	auto& env = python_test_env();
	auto get_item = env.builtins.load_entity(
		"callable=list.__getitem__,instance_required",
		{metaffi_handle_type, metaffi_int64_type},
		{metaffi_handle_type});
	auto [handle] = get_item.call<cdt_metaffi_handle*>(*_handle.get(), static_cast<int64_t>(index));
	return PyHandle(handle);
}

metaffi_variant PyList::get_any(metaffi_size index) const
{
	auto& env = python_test_env();
	auto get_item = env.builtins.load_entity(
		"callable=list.__getitem__,instance_required",
		{metaffi_handle_type, metaffi_int64_type},
		{metaffi_any_type});
	auto [val] = get_item.call<metaffi_variant>(*_handle.get(), static_cast<int64_t>(index));
	return val;
}

PyDict PyDict::create()
{
	auto& env = python_test_env();
	auto ctor = env.builtins.load_entity("callable=dict", {}, {metaffi_handle_type});
	auto [handle] = ctor.call<cdt_metaffi_handle*>();
	return PyDict(PyHandle(handle));
}

PyDict::PyDict(PyHandle handle)
	: _handle(std::move(handle))
{
}

cdt_metaffi_handle* PyDict::handle() const
{
	return _handle.get();
}

PySet PySet::create()
{
	auto& env = python_test_env();
	auto ctor = env.builtins.load_entity("callable=set", {}, {metaffi_handle_type});
	auto [handle] = ctor.call<cdt_metaffi_handle*>();
	return PySet(PyHandle(handle));
}

PySet::PySet(PyHandle handle)
	: _handle(std::move(handle))
{
}

cdt_metaffi_handle* PySet::handle() const
{
	return _handle.get();
}

PyTuple PyTuple::from_ints(const std::vector<int64_t>& items)
{
	auto& env = python_test_env();
	auto ctor = env.builtins.load_entity("callable=tuple", {metaffi_int64_array_type}, {metaffi_handle_type});
	auto [handle] = ctor.call<cdt_metaffi_handle*>(items);
	return PyTuple(PyHandle(handle));
}

PyTuple::PyTuple(PyHandle handle)
	: _handle(std::move(handle))
{
}

cdt_metaffi_handle* PyTuple::handle() const
{
	return _handle.get();
}

PyFrozenSet PyFrozenSet::from_ints(const std::vector<int64_t>& items)
{
	auto& env = python_test_env();
	auto ctor = env.builtins.load_entity("callable=frozenset", {metaffi_int64_array_type}, {metaffi_handle_type});
	auto [handle] = ctor.call<cdt_metaffi_handle*>(items);
	return PyFrozenSet(PyHandle(handle));
}

PyFrozenSet::PyFrozenSet(PyHandle handle)
	: _handle(std::move(handle))
{
}

cdt_metaffi_handle* PyFrozenSet::handle() const
{
	return _handle.get();
}
