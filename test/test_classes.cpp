#include <doctest/doctest.h>

#include "python3_test_env.h"
#include "python3_wrappers.h"

#include <vector>

TEST_CASE("classes and objects")
{
	auto& env = python_test_env();

	auto some_class_get = env.module_dir.load_entity(
		"attribute=SomeClass,getter",
		{},
		{metaffi_handle_type});
	auto [some_class_ptr] = some_class_get.call<cdt_metaffi_handle*>();
	PyHandle some_class_type(some_class_ptr);

	auto some_new = env.module_dir.load_entity(
		"callable=SomeClass.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [some_inst_ptr] = some_new.call<cdt_metaffi_handle*>(*some_class_type.get());
	PyHandle some_inst(some_inst_ptr);

	auto some_init = env.module_dir.load_entity(
		"callable=SomeClass.__init__,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{});
	some_init.call<>(*some_inst.get(), std::string("x"));

	auto some_print = env.module_dir.load_entity(
		"callable=SomeClass.print,instance_required",
		{metaffi_handle_type},
		{metaffi_string8_type});
	auto [print_str] = some_print.call<std::string>(*some_inst.get());
	CHECK(print_str.find("x") != std::string::npos);

	auto some_getattr = env.module_dir.load_entity(
		"callable=SomeClass.__getattribute__,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{metaffi_string8_type});
	auto [name_val] = some_getattr.call<std::string>(*some_inst.get(), std::string("name"));
	CHECK(name_val == "x");

	auto some_setattr = env.module_dir.load_entity(
		"callable=SomeClass.__setattr__,instance_required",
		{metaffi_handle_type, metaffi_string8_type, metaffi_string8_type},
		{});
	some_setattr.call<>(*some_inst.get(), std::string("name"), std::string("y"));
	auto [name_val2] = some_getattr.call<std::string>(*some_inst.get(), std::string("name"));
	CHECK(name_val2 == "y");

	auto some_str = env.module_dir.load_entity(
		"callable=SomeClass.__str__,instance_required",
		{metaffi_handle_type},
		{metaffi_string8_type});
	auto [str_val] = some_str.call<std::string>(*some_inst.get());
	CHECK(str_val == "SomeClass(y)");

	auto some_eq = env.module_dir.load_entity(
		"callable=SomeClass.__eq__,instance_required",
		{metaffi_handle_type, metaffi_handle_type},
		{metaffi_bool_type});

	auto [same_ptr] = some_new.call<cdt_metaffi_handle*>(*some_class_type.get());
	PyHandle same_inst(same_ptr);
	some_init.call<>(*same_inst.get(), std::string("y"));
	auto [eq_same] = some_eq.call<bool>(*some_inst.get(), *same_inst.get());
	CHECK(eq_same);

	auto [diff_ptr] = some_new.call<cdt_metaffi_handle*>(*some_class_type.get());
	PyHandle diff_inst(diff_ptr);
	some_init.call<>(*diff_inst.get(), std::string("z"));
	auto [eq_diff] = some_eq.call<bool>(*some_inst.get(), *diff_inst.get());
	CHECK(!eq_diff);

	auto get_some = env.module_dir.load_entity(
		"callable=get_some_classes",
		{},
		{metaffi_handle_array_type});
	auto [some_ptrs] = get_some.call<std::vector<cdt_metaffi_handle*>>();
	CHECK(some_ptrs.size() == 3);

	std::vector<PyHandle> some_handles;
	some_handles.reserve(some_ptrs.size());
	for(auto* ptr : some_ptrs)
	{
		some_handles.emplace_back(ptr);
	}

	std::vector<cdt_metaffi_handle> some_values;
	some_values.reserve(some_handles.size());
	for(const auto& handle : some_handles)
	{
		some_values.push_back(*handle.get());
	}

	auto expect_some = env.module_dir.load_entity(
		"callable=expect_three_some_classes",
		{metaffi_handle_array_type},
		{});
	expect_some.call<>(some_values);

	auto testmap_get = env.module_dir.load_entity(
		"attribute=TestMap,getter",
		{},
		{metaffi_handle_type});
	auto [testmap_class_ptr] = testmap_get.call<cdt_metaffi_handle*>();
	PyHandle testmap_class(testmap_class_ptr);

	auto testmap_new = env.module_dir.load_entity(
		"callable=TestMap.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [testmap_inst_ptr] = testmap_new.call<cdt_metaffi_handle*>(*testmap_class.get());
	PyHandle testmap_inst(testmap_inst_ptr);

	auto testmap_init = env.module_dir.load_entity(
		"callable=TestMap.__init__,instance_required",
		{metaffi_handle_type},
		{});
	testmap_init.call<>(*testmap_inst.get());

	auto map_set = env.module_dir.load_entity(
		"callable=TestMap.set,instance_required",
		{metaffi_handle_type, metaffi_string8_type, metaffi_int64_type},
		{});
	map_set.call<>(*testmap_inst.get(), std::string("k"), int64_t(1));
	auto map_contains = env.module_dir.load_entity(
		"callable=TestMap.contains,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{metaffi_bool_type});
	auto [contains] = map_contains.call<bool>(*testmap_inst.get(), std::string("k"));
	CHECK(contains);
	auto map_get = env.module_dir.load_entity(
		"callable=TestMap.get,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{metaffi_any_type});
	auto [map_val] = map_get.call<metaffi_variant>(*testmap_inst.get(), std::string("k"));
	CHECK(std::holds_alternative<metaffi_int64>(map_val));
	CHECK(std::get<metaffi_int64>(map_val) == 1);

	auto base_get = env.module_dir.load_entity(
		"attribute=BaseClass,getter",
		{},
		{metaffi_handle_type});
	auto [base_class_ptr] = base_get.call<cdt_metaffi_handle*>();
	PyHandle base_class(base_class_ptr);

	auto base_new = env.module_dir.load_entity(
		"callable=BaseClass.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [base_inst_ptr] = base_new.call<cdt_metaffi_handle*>(*base_class.get());
	PyHandle base_inst(base_inst_ptr);

	auto base_init = env.module_dir.load_entity(
		"callable=BaseClass.__init__,instance_required",
		{metaffi_handle_type, metaffi_int64_type},
		{});
	base_init.call<>(*base_inst.get(), int64_t(3));

	auto base_method = env.module_dir.load_entity(
		"callable=BaseClass.base_method,instance_required",
		{metaffi_handle_type},
		{metaffi_int64_type});
	auto [base_val] = base_method.call<int64_t>(*base_inst.get());
	CHECK(base_val == 3);

	auto base_make_default = env.module_dir.load_entity(
		"callable=BaseClass.make_default",
		{},
		{metaffi_handle_type});
	auto [base_default_ptr] = base_make_default.call<cdt_metaffi_handle*>();
	PyHandle base_default(base_default_ptr);
	auto [base_default_val] = base_method.call<int64_t>(*base_default.get());
	CHECK(base_default_val == 7);

	auto base_static = env.module_dir.load_entity(
		"callable=BaseClass.static_value",
		{},
		{metaffi_int64_type});
	auto [static_val] = base_static.call<int64_t>();
	CHECK(static_val == 42);

	auto derived_get = env.module_dir.load_entity(
		"attribute=DerivedClass,getter",
		{},
		{metaffi_handle_type});
	auto [derived_class_ptr] = derived_get.call<cdt_metaffi_handle*>();
	PyHandle derived_class(derived_class_ptr);

	auto derived_new = env.module_dir.load_entity(
		"callable=DerivedClass.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [derived_ptr] = derived_new.call<cdt_metaffi_handle*>(*derived_class.get());
	PyHandle derived_inst(derived_ptr);

	auto derived_init = env.module_dir.load_entity(
		"callable=DerivedClass.__init__,instance_required",
		{metaffi_handle_type, metaffi_int64_type, metaffi_string8_type},
		{});
	derived_init.call<>(*derived_inst.get(), int64_t(2), std::string("extra"));

	auto derived_method = env.module_dir.load_entity(
		"callable=DerivedClass.derived_method,instance_required",
		{metaffi_handle_type},
		{metaffi_string8_type});
	auto [derived_str] = derived_method.call<std::string>(*derived_inst.get());
	CHECK(derived_str == "extra");

	auto derived_base_method = env.module_dir.load_entity(
		"callable=DerivedClass.base_method,instance_required",
		{metaffi_handle_type},
		{metaffi_int64_type});
	auto [derived_base_val] = derived_base_method.call<int64_t>(*derived_inst.get());
	CHECK(derived_base_val == 4);

	auto nested_get = env.module_dir.load_entity(
		"attribute=NestedContainer,getter",
		{},
		{metaffi_handle_type});
	auto [nested_class_ptr] = nested_get.call<cdt_metaffi_handle*>();
	PyHandle nested_class(nested_class_ptr);

	auto nested_new = env.module_dir.load_entity(
		"callable=NestedContainer.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [nested_ptr] = nested_new.call<cdt_metaffi_handle*>(*nested_class.get());
	PyHandle nested_inst(nested_ptr);

	auto nested_init = env.module_dir.load_entity(
		"callable=NestedContainer.__init__,instance_required",
		{metaffi_handle_type, metaffi_int64_type},
		{});
	nested_init.call<>(*nested_inst.get(), int64_t(5));

	auto nested_getattr = env.module_dir.load_entity(
		"callable=NestedContainer.__getattribute__,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{metaffi_handle_type});
	auto [inner_ptr] = nested_getattr.call<cdt_metaffi_handle*>(*nested_inst.get(), std::string("inner"));
	PyHandle inner_handle(inner_ptr);

	auto inner_get_value = env.module_dir.load_entity(
		"callable=NestedContainer.Inner.get_value,instance_required",
		{metaffi_handle_type},
		{metaffi_int64_type});
	auto [inner_val] = inner_get_value.call<int64_t>(*inner_handle.get());
	CHECK(inner_val == 5);

	auto callable_get = env.module_dir.load_entity(
		"attribute=CallableClass,getter",
		{},
		{metaffi_handle_type});
	auto [callable_class_ptr] = callable_get.call<cdt_metaffi_handle*>();
	PyHandle callable_class(callable_class_ptr);

	auto callable_new = env.module_dir.load_entity(
		"callable=CallableClass.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [callable_ptr] = callable_new.call<cdt_metaffi_handle*>(*callable_class.get());
	PyHandle callable_inst(callable_ptr);

	auto callable_init = env.module_dir.load_entity(
		"callable=CallableClass.__init__,instance_required",
		{metaffi_handle_type},
		{});
	callable_init.call<>(*callable_inst.get());

	auto call_method = env.module_dir.load_entity(
		"callable=CallableClass.__call__,instance_required",
		{metaffi_handle_type, metaffi_int64_type, metaffi_int64_type},
		{metaffi_int64_type});
	auto [call_val] = call_method.call<int64_t>(*callable_inst.get(), int64_t(2), int64_t(3));
	CHECK(call_val == 5);

	auto iter_get = env.module_dir.load_entity(
		"attribute=IteratorClass,getter",
		{},
		{metaffi_handle_type});
	auto [iter_class_ptr] = iter_get.call<cdt_metaffi_handle*>();
	PyHandle iter_class(iter_class_ptr);

	auto iter_new = env.module_dir.load_entity(
		"callable=IteratorClass.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [iter_ptr] = iter_new.call<cdt_metaffi_handle*>(*iter_class.get());
	PyHandle iter_inst(iter_ptr);

	auto iter_init = env.module_dir.load_entity(
		"callable=IteratorClass.__init__,instance_required",
		{metaffi_handle_type, metaffi_int64_type},
		{});
	iter_init.call<>(*iter_inst.get(), int64_t(3));

	auto iter_entity = env.module_dir.load_entity(
		"callable=IteratorClass.__iter__,instance_required",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [iter_obj_ptr] = iter_entity.call<cdt_metaffi_handle*>(*iter_inst.get());
	PyHandle iter_obj(iter_obj_ptr);

	auto next_entity = env.module_dir.load_entity(
		"callable=IteratorClass.__next__,instance_required",
		{metaffi_handle_type},
		{metaffi_int64_type});
	auto [n0] = next_entity.call<int64_t>(*iter_obj.get());
	auto [n1] = next_entity.call<int64_t>(*iter_obj.get());
	auto [n2] = next_entity.call<int64_t>(*iter_obj.get());
	CHECK(n0 == 0);
	CHECK(n1 == 1);
	CHECK(n2 == 2);

	auto ctx_get = env.module_dir.load_entity(
		"attribute=ContextManager,getter",
		{},
		{metaffi_handle_type});
	auto [ctx_class_ptr] = ctx_get.call<cdt_metaffi_handle*>();
	PyHandle ctx_class(ctx_class_ptr);

	auto ctx_new = env.module_dir.load_entity(
		"callable=ContextManager.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [ctx_ptr] = ctx_new.call<cdt_metaffi_handle*>(*ctx_class.get());
	PyHandle ctx_inst(ctx_ptr);

	auto ctx_init = env.module_dir.load_entity(
		"callable=ContextManager.__init__,instance_required",
		{metaffi_handle_type},
		{});
	ctx_init.call<>(*ctx_inst.get());

	auto ctx_enter = env.module_dir.load_entity(
		"callable=ContextManager.__enter__,instance_required",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [ctx_self_ptr] = ctx_enter.call<cdt_metaffi_handle*>(*ctx_inst.get());
	PyHandle ctx_self(ctx_self_ptr);

	auto ctx_getattr = env.module_dir.load_entity(
		"callable=ContextManager.__getattribute__,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{metaffi_bool_type});
	auto [entered] = ctx_getattr.call<bool>(*ctx_self.get(), std::string("entered"));
	CHECK(entered);

	auto ctx_exit = env.module_dir.load_entity(
		"callable=ContextManager.__exit__,instance_required",
		{metaffi_handle_type, metaffi_any_type, metaffi_any_type, metaffi_any_type},
		{metaffi_bool_type});
	auto [exit_res] = ctx_exit.call<bool>(*ctx_inst.get(), nullptr, nullptr, nullptr);
	CHECK(exit_res == false);

	auto [exited] = ctx_getattr.call<bool>(*ctx_inst.get(), std::string("exited"));
	CHECK(exited);

	auto data_get = env.module_dir.load_entity(
		"attribute=DataRecord,getter",
		{},
		{metaffi_handle_type});
	auto [data_class_ptr] = data_get.call<cdt_metaffi_handle*>();
	PyHandle data_class(data_class_ptr);

	auto data_new = env.module_dir.load_entity(
		"callable=DataRecord.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [data_ptr] = data_new.call<cdt_metaffi_handle*>(*data_class.get());
	PyHandle data_inst(data_ptr);

	auto data_init = env.module_dir.load_entity(
		"callable=DataRecord.__init__,instance_required",
		{metaffi_handle_type, metaffi_int64_type, metaffi_string8_type},
		{});
	data_init.call<>(*data_inst.get(), int64_t(1), std::string("name"));

	auto data_getattr = env.module_dir.load_entity(
		"callable=DataRecord.__getattribute__,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{metaffi_any_type});
	auto [id_val] = data_getattr.call<metaffi_variant>(*data_inst.get(), std::string("id"));
	CHECK(std::holds_alternative<metaffi_int64>(id_val));
	CHECK(std::get<metaffi_int64>(id_val) == 1);

	auto [name_any] = data_getattr.call<metaffi_variant>(*data_inst.get(), std::string("name"));
	CHECK(std::holds_alternative<metaffi_string8>(name_any));
	CHECK(take_string8(std::get<metaffi_string8>(name_any)) == "name");

	auto color_red_get = env.module_dir.load_entity(
		"attribute=Color.RED,getter",
		{},
		{metaffi_handle_type});
	auto [red_ptr] = color_red_get.call<cdt_metaffi_handle*>();
	PyHandle red(red_ptr);

	auto color_str = env.module_dir.load_entity(
		"callable=Color.__str__,instance_required",
		{metaffi_handle_type},
		{metaffi_string8_type});
	auto [red_text] = color_str.call<std::string>(*red.get());
	CHECK(red_text == "Color.RED");

	auto color_green_get = env.module_dir.load_entity(
		"attribute=Color.GREEN,getter",
		{},
		{metaffi_handle_type});
	auto [green_ptr] = color_green_get.call<cdt_metaffi_handle*>();
	PyHandle green(green_ptr);
	auto [green_text] = color_str.call<std::string>(*green.get());
	CHECK(green_text == "Color.GREEN");

	auto color_blue_get = env.module_dir.load_entity(
		"attribute=Color.BLUE,getter",
		{},
		{metaffi_handle_type});
	auto [blue_ptr] = color_blue_get.call<cdt_metaffi_handle*>();
	PyHandle blue(blue_ptr);
	auto [blue_text] = color_str.call<std::string>(*blue.get());
	CHECK(blue_text == "Color.BLUE");
}

TEST_CASE("custom error object")
{
	auto& env = python_test_env();

	auto err_get = env.module_dir.load_entity(
		"attribute=CustomError,getter",
		{},
		{metaffi_handle_type});
	auto [err_class_ptr] = err_get.call<cdt_metaffi_handle*>();
	PyHandle err_class(err_class_ptr);

	auto err_new = env.module_dir.load_entity(
		"callable=CustomError.__new__",
		{metaffi_handle_type},
		{metaffi_handle_type});
	auto [err_ptr] = err_new.call<cdt_metaffi_handle*>(*err_class.get());
	PyHandle err_inst(err_ptr);

	auto err_init = env.module_dir.load_entity(
		"callable=CustomError.__init__,instance_required",
		{metaffi_handle_type, metaffi_string8_type},
		{});
	err_init.call<>(*err_inst.get(), std::string("boom"));

	auto err_str = env.module_dir.load_entity(
		"callable=CustomError.__str__,instance_required",
		{metaffi_handle_type},
		{metaffi_string8_type});
	auto [err_text] = err_str.call<std::string>(*err_inst.get());
	CHECK(err_text == "boom");
}
