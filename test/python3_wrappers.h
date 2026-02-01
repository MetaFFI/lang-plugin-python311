#pragma once

#include "python3_test_env.h"

#include <runtime/metaffi_primitives.h>
#include <runtime/xllr_capi_loader.h>

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

class PyHandle
{
public:
	explicit PyHandle(cdt_metaffi_handle* handle = nullptr);
	~PyHandle();

	PyHandle(const PyHandle&) = delete;
	PyHandle& operator=(const PyHandle&) = delete;

	PyHandle(PyHandle&& other) noexcept;
	PyHandle& operator=(PyHandle&& other) noexcept;

	[[nodiscard]] cdt_metaffi_handle* get() const;
	[[nodiscard]] bool is_null() const;
	cdt_metaffi_handle* release();

private:
	cdt_metaffi_handle* _handle = nullptr;
};

std::string take_string8(metaffi_string8 value);

class PyList
{
public:
	static PyList create();

	explicit PyList(PyHandle handle);

	[[nodiscard]] cdt_metaffi_handle* handle() const;
	[[nodiscard]] metaffi_size size() const;

	template<typename T>
	void append(T&& value)
	{
		auto& env = python_test_env();
		auto append_entity = env.builtins.load_entity(
			"callable=list.append,instance_required",
			{metaffi_handle_type, metaffi_any_type},
			{});
		append_entity.call<>(*handle(), std::forward<T>(value));
	}

	[[nodiscard]] PyHandle get_handle(metaffi_size index) const;
	[[nodiscard]] metaffi_variant get_any(metaffi_size index) const;

private:
	PyHandle _handle;
};

class PyDict
{
public:
	static PyDict create();

	explicit PyDict(PyHandle handle);

	[[nodiscard]] cdt_metaffi_handle* handle() const;

	template<typename K, typename V>
	void set(K&& key, V&& value)
	{
		auto& env = python_test_env();
		auto set_item = env.builtins.load_entity(
			"callable=dict.__setitem__,instance_required",
			{metaffi_handle_type, metaffi_any_type, metaffi_any_type},
			{});
		set_item.call<>(*handle(), std::forward<K>(key), std::forward<V>(value));
	}

	template<typename K>
	[[nodiscard]] PyHandle get_handle(K&& key) const
	{
		auto& env = python_test_env();
		auto get_item = env.builtins.load_entity(
			"callable=dict.__getitem__,instance_required",
			{metaffi_handle_type, metaffi_any_type},
			{metaffi_handle_type});
		auto [handle] = get_item.call<cdt_metaffi_handle*>(*_handle.get(), std::forward<K>(key));
		return PyHandle(handle);
	}

	template<typename K>
	[[nodiscard]] metaffi_variant get_any(K&& key) const
	{
		auto& env = python_test_env();
		auto get_item = env.builtins.load_entity(
			"callable=dict.__getitem__,instance_required",
			{metaffi_handle_type, metaffi_any_type},
			{metaffi_any_type});
		auto [val] = get_item.call<metaffi_variant>(*_handle.get(), std::forward<K>(key));
		return val;
	}

private:
	PyHandle _handle;
};

class PySet
{
public:
	static PySet create();

	explicit PySet(PyHandle handle);

	[[nodiscard]] cdt_metaffi_handle* handle() const;

	template<typename T>
	void add(T&& value)
	{
		auto& env = python_test_env();
		auto add_entity = env.builtins.load_entity(
			"callable=set.add,instance_required",
			{metaffi_handle_type, metaffi_any_type},
			{});
		add_entity.call<>(*handle(), std::forward<T>(value));
	}

private:
	PyHandle _handle;
};

class PyTuple
{
public:
	static PyTuple from_ints(const std::vector<int64_t>& items);

	explicit PyTuple(PyHandle handle);

	[[nodiscard]] cdt_metaffi_handle* handle() const;

private:
	PyHandle _handle;
};

class PyFrozenSet
{
public:
	static PyFrozenSet from_ints(const std::vector<int64_t>& items);

	explicit PyFrozenSet(PyHandle handle);

	[[nodiscard]] cdt_metaffi_handle* handle() const;

private:
	PyHandle _handle;
};
