#include <doctest/doctest.h>

#include "python3_test_env.h"
#include "python3_wrappers.h"

TEST_CASE("generator and async")
{
	auto& env = python_test_env();

	auto gen_count = env.module_dir.load_entity(
		"callable=generator_count",
		{metaffi_int64_type},
		{metaffi_handle_type});
	auto [gen_ptr] = gen_count.call<cdt_metaffi_handle*>(int64_t(3));
	PyHandle gen_handle(gen_ptr);

	auto next_entity = env.builtins.load_entity(
		"callable=next",
		{metaffi_handle_type},
		{metaffi_int64_type});
	auto [n0] = next_entity.call<int64_t>(*gen_handle.get());
	auto [n1] = next_entity.call<int64_t>(*gen_handle.get());
	auto [n2] = next_entity.call<int64_t>(*gen_handle.get());
	CHECK(n0 == 0);
	CHECK(n1 == 1);
	CHECK(n2 == 2);

	auto async_add = env.module_dir.load_entity(
		"callable=async_add",
		{metaffi_int64_type, metaffi_int64_type},
		{metaffi_handle_type});
	auto [coro_ptr] = async_add.call<cdt_metaffi_handle*>(int64_t(2), int64_t(3));
	PyHandle coro_handle(coro_ptr);

	auto run_entity = env.asyncio.load_entity(
		"callable=run",
		{metaffi_handle_type},
		{metaffi_int64_type});
	auto [async_sum] = run_entity.call<int64_t>(*coro_handle.get());
	CHECK(async_sum == 5);
}
