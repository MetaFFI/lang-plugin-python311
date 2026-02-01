#include <doctest/doctest.h>

#include "python3_test_env.h"
#include "python3_wrappers.h"

#include <vector>

TEST_CASE("module state")
{
	auto& env = python_test_env();

	auto const_get = env.module_dir.load_entity(
		"attribute=CONSTANT_FIVE_SECONDS,getter",
		{},
		{metaffi_int64_type});
	auto [const_val] = const_get.call<int64_t>();
	CHECK(const_val == 5);

	auto set_counter = env.module_dir.load_entity(
		"callable=set_counter",
		{metaffi_int64_type},
		{});
	set_counter.call<>(int64_t(0));

	auto inc_counter = env.module_dir.load_entity(
		"callable=inc_counter",
		{metaffi_int64_type},
		{metaffi_int64_type});
	auto [count1] = inc_counter.call<int64_t>(int64_t(1));
	CHECK(count1 == 1);

	auto get_counter = env.module_dir.load_entity(
		"callable=get_counter",
		{},
		{metaffi_int64_type});
	auto [count2] = get_counter.call<int64_t>();
	CHECK(count2 == 1);

	auto set_global = env.module_dir.load_entity(
		"callable=set_global_value",
		{metaffi_string8_type, metaffi_any_type},
		{});
	set_global.call<>(std::string("k"), std::string("v"));

	auto get_global = env.module_dir.load_entity(
		"callable=get_global_value",
		{metaffi_string8_type},
		{metaffi_any_type});
	auto [val] = get_global.call<metaffi_variant>(std::string("k"));
	CHECK(std::holds_alternative<metaffi_string8>(val));
	CHECK(take_string8(std::get<metaffi_string8>(val)) == "v");

	auto closure_factory = env.module_dir.load_entity(
		"callable=closure_factory",
		{metaffi_int64_type},
		{metaffi_callable_type});
	auto [closure_ptr] = closure_factory.call<cdt_metaffi_callable*>(int64_t(10));
	metaffi::api::MetaFFICallable closure_owner(closure_ptr, env.runtime.runtime_plugin());

	auto [first] = closure_owner.call<int64_t>(int64_t(1));
	auto [second] = closure_owner.call<int64_t>(int64_t(2));
	CHECK(first == 11);
	CHECK(second == 13);
}
