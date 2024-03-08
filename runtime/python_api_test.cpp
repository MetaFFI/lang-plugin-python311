#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <runtime/runtime_plugin_api.h>
#include <filesystem>
#include <runtime/cdts_wrapper.h>
#include "runtime_id.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

TEST_CASE( "python3 runtime api", "[python3runtime]" )
{
	REQUIRE(std::getenv("METAFFI_HOME") != nullptr);
	
	std::filesystem::path module_path(__FILE__);
	module_path = module_path.parent_path();
	module_path.append("./test/runtime_test_target.py");
	char* err = nullptr;
	uint32_t err_len = 0;
	
	SECTION("Load Runtime")
	{
		load_runtime(&err, &err_len);
		
		if(err){
			FAIL(err);
		}
		
		REQUIRE(err_len == 0);
	}

	SECTION("runtime_test_target.hello_world")
	{
		std::string function_path = "callable=hello_world";
		void** phello_world = load_function(module_path.string().c_str(), module_path.string().length(),
											function_path.c_str(), function_path.length(),
											nullptr, nullptr,
											  0, 0,
											  &err, &err_len);

		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(phello_world[0] != nullptr);
		REQUIRE(phello_world[1] != nullptr);

		uint64_t long_err_len = 0;
		((void(*)(void*,char**, uint64_t*))phello_world[0])(phello_world[1], &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
	}

	SECTION("runtime_test_target.returns_an_error")
	{
		std::string function_path = "callable=returns_an_error";
		void** preturns_an_error = load_function(module_path.string().c_str(), module_path.string().length(),
		                                    function_path.c_str(), function_path.length(),
											nullptr, nullptr,
		                                    0, 0,
		                                    &err, &err_len);

		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(preturns_an_error[0] != nullptr);
		REQUIRE(preturns_an_error[1] != nullptr);

		uint64_t long_err_len = 0;
		((void(*)(void*,char**, uint64_t*))preturns_an_error[0])(preturns_an_error[1], &err, &long_err_len);
		REQUIRE(err != nullptr);
		REQUIRE(long_err_len > 0);

		SUCCEED(std::string(err, long_err_len).c_str());
	}

	SECTION("runtime_test_target.div_integers")
	{
		std::string function_path = "callable=div_integers";
		metaffi_type_info p1, p2, r1;
		p1.type = metaffi_int64_type;
		p2.type = metaffi_int64_type;
		r1.type = metaffi_float32_type;
		
		metaffi_type_info params_types[] = {p1, p2};
		metaffi_type_info retvals_types[] = {r1};
		
		uint8_t params_count = 2;
		uint8_t retvals_count = 1;
		
		void** pdiv_integers = load_function(module_path.string().c_str(), module_path.string().length(),
		                                         function_path.c_str(), function_path.length(),
		                                        params_types, retvals_types,
		                                     params_count, retvals_count,
		                                         &err, &err_len);

		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pdiv_integers[0] != nullptr);
		REQUIRE(pdiv_integers[1] != nullptr);
		
		
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(params_count, retvals_count);
		metaffi::runtime::cdts_wrapper wrapper(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper[0]->type = metaffi_int64_type;
		wrapper[0]->cdt_val.metaffi_int64_val.val = 10;
		wrapper[1]->type = metaffi_int64_type;
		wrapper[1]->cdt_val.metaffi_int64_val.val = 2;
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pdiv_integers[0])(pdiv_integers[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_ret[0]->type == metaffi_float32_type);
		REQUIRE(wrapper_ret[0]->cdt_val.metaffi_float32_val.val == 5.0);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
	}
	
	SECTION("runtime_test_target.join_strings")
	{
		std::string function_path = "callable=join_strings";
		metaffi_type_info params_types[] = {metaffi::runtime::make_type_with_options(metaffi_string8_array_type)};
		metaffi_type_info retvals_types[] = {metaffi::runtime::make_type_with_options(metaffi_string8_type)};
		
		void** join_strings = load_function(module_path.string().c_str(), module_path.string().length(),
		                                     function_path.c_str(), function_path.length(),
		                                     params_types, retvals_types,
		                                     1, 1,
		                                     &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(join_strings[0] != nullptr);
		REQUIRE(join_strings[1] != nullptr);
		
		
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(1, 1);
		metaffi::runtime::cdts_wrapper wrapper(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		metaffi_size array_dimensions;
		metaffi_size array_length[] = {3};
		metaffi_string8 values[] = {(metaffi_string8)"one", (metaffi_string8)"two", (metaffi_string8)"three"};
		metaffi_size vals_length[] = {strlen("one"), strlen("two"), strlen("three")};
		wrapper[0]->type = metaffi_string8_array_type;
		wrapper[0]->cdt_val.metaffi_string8_array_val.dimensions = 1;
		wrapper[0]->cdt_val.metaffi_string8_array_val.dimensions_lengths = (metaffi_size*)array_length;
		wrapper[0]->cdt_val.metaffi_string8_array_val.vals = values;
		wrapper[0]->cdt_val.metaffi_string8_array_val.vals_sizes = vals_length;
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))join_strings[0])(join_strings[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_ret[0]->type == metaffi_string8_type);
		
		std::string returned(wrapper_ret[0]->cdt_val.metaffi_string8_val.val, wrapper_ret[0]->cdt_val.metaffi_string8_val.length);
		REQUIRE(returned == "one,two,three");
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
	}
	
	SECTION("runtime_test_target.testmap.set_get_contains")
	{
		// create new testmap
		std::string function_path = "callable=testmap";
		metaffi_type_info retvals_types[] = {metaffi::runtime::make_type_with_options(metaffi_handle_type)};
		
		void** pnew_testmap = load_function(module_path.string().c_str(), module_path.string().length(),
		                                    function_path.c_str(), function_path.length(),
		                                    nullptr, retvals_types,
		                                    0, 1,
		                                    &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pnew_testmap[0] != nullptr);
		REQUIRE(pnew_testmap[1] != nullptr);
		
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pnew_testmap[0])(pnew_testmap[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_ret[0]->type == metaffi_handle_type);
		REQUIRE(wrapper_ret[0]->cdt_val.metaffi_handle_val.val != nullptr);
		REQUIRE(wrapper_ret[0]->cdt_val.metaffi_handle_val.runtime_id == PYTHON311_RUNTIME_ID);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
		
		metaffi_handle testmap_instance = wrapper_ret[0]->cdt_val.metaffi_handle_val.val;
		
		// set
		function_path = "callable=testmap.set,instance_required";
		metaffi_type_info params_types[] = {metaffi::runtime::make_type_with_options(metaffi_handle_type),
		                                          metaffi::runtime::make_type_with_options(metaffi_string8_type),
		                                          metaffi::runtime::make_type_with_options(metaffi_any_type)};
		
		void** p_testmap_set = load_function(module_path.string().c_str(), module_path.string().length(),
		                                    function_path.c_str(), function_path.length(),
		                                     params_types, nullptr,
		                                    3, 0,
		                                    &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(p_testmap_set[0] != nullptr);
		REQUIRE(p_testmap_set[1] != nullptr);
		
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(3, 0);
		metaffi::runtime::cdts_wrapper wrapper(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper[0]->type = metaffi_handle_type;
		wrapper[0]->cdt_val.metaffi_handle_val.val = testmap_instance;
		wrapper[0]->cdt_val.metaffi_handle_val.runtime_id = PYTHON311_RUNTIME_ID;
		wrapper[1]->type = metaffi_string8_type;
		wrapper[1]->cdt_val.metaffi_string8_val.val = (char*)"key";
		wrapper[1]->cdt_val.metaffi_string8_val.length = strlen("key");
		wrapper[2]->type = metaffi_int64_type;
		wrapper[2]->cdt_val.metaffi_int64_val.val = 42;
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))p_testmap_set[0])(p_testmap_set[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
		
		// contains
		function_path = "callable=testmap.contains,instance_required";
		params_types[0].type = metaffi_handle_type;
		params_types[1].type = metaffi_string8_type;
		retvals_types[0].type = metaffi_bool_type;
		
		void** p_testmap_contains = load_function(module_path.string().c_str(), module_path.string().length(),
		                                     function_path.c_str(), function_path.length(),
	                                          params_types, retvals_types,
		                                     2, 1,
		                                     &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(p_testmap_contains[0] != nullptr);
		REQUIRE(p_testmap_contains[1] != nullptr);
		
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(2, 1);
		metaffi::runtime::cdts_wrapper wrapper_contains_params(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper_contains_params[0]->type = metaffi_handle_type;
		wrapper_contains_params[0]->cdt_val.metaffi_handle_val.val = testmap_instance;
		wrapper_contains_params[0]->cdt_val.metaffi_handle_val.runtime_id = PYTHON311_RUNTIME_ID;
		wrapper_contains_params[1]->type = metaffi_string8_type;
		wrapper_contains_params[1]->cdt_val.metaffi_string8_val.val = (char*)"key";
		wrapper_contains_params[1]->cdt_val.metaffi_string8_val.length = strlen("key");
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))p_testmap_contains[0])(p_testmap_contains[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_contains_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_contains_ret[0]->type == metaffi_bool_type);
		REQUIRE(wrapper_contains_ret[0]->cdt_val.metaffi_bool_val.val != 0);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
		
		// get
		function_path = "callable=testmap.get,instance_required";
		params_types[0].type = metaffi_handle_type;
		params_types[1].type = metaffi_string8_type;
		retvals_types[0].type = metaffi_any_type;
		
		void** p_testmap_get = load_function(module_path.string().c_str(), module_path.string().length(),
		                                          function_path.c_str(), function_path.length(),
		                                          nullptr, retvals_types,
		                                          1, 1,
		                                          &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(p_testmap_get[0] != nullptr);
		REQUIRE(p_testmap_get[1] != nullptr);
		
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(2, 1);
		metaffi::runtime::cdts_wrapper wrapper_get_params(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper_get_params[0]->type = metaffi_handle_type;
		wrapper_get_params[0]->cdt_val.metaffi_handle_val.val = testmap_instance;
		wrapper_get_params[0]->cdt_val.metaffi_handle_val.runtime_id = PYTHON311_RUNTIME_ID;
		wrapper_get_params[1]->type = metaffi_string8_type;
		wrapper_get_params[1]->cdt_val.metaffi_string8_val.val = (char*)"key";
		wrapper_get_params[1]->cdt_val.metaffi_string8_val.length = strlen("key");
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))p_testmap_get[0])(p_testmap_get[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_get_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_get_ret[0]->type == metaffi_int64_type);
		REQUIRE(wrapper_get_ret[0]->cdt_val.metaffi_int64_val.val == 42);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
	}
	
	SECTION("runtime_test_target.testmap.set_get_contains_cpp_object")
	{
		// create new testmap
		std::string function_path = "callable=testmap";
		metaffi_type_info retvals_types[] = {{metaffi_handle_type}};
		
		void** pnew_testmap = load_function(module_path.string().c_str(), module_path.string().length(),
		                                    function_path.c_str(), function_path.length(),
		                                    nullptr, retvals_types,
		                                    0, 1,
		                                    &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pnew_testmap[0] != nullptr);
		REQUIRE(pnew_testmap[1] != nullptr);
		
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pnew_testmap[0])(pnew_testmap[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_ret[0]->type == metaffi_handle_type);
		REQUIRE(wrapper_ret[0]->cdt_val.metaffi_handle_val.val != nullptr);
		REQUIRE(wrapper_ret[0]->cdt_val.metaffi_handle_val.runtime_id == PYTHON311_RUNTIME_ID);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
		
		metaffi_handle testmap_instance = wrapper_ret[0]->cdt_val.metaffi_handle_val.val;
		
		// set
		function_path = "callable=testmap.set,instance_required";
		metaffi_type_info params_types[] = {{metaffi_handle_type}, {metaffi_string8_type}, {metaffi_any_type}};
		
		void** p_testmap_set = load_function(module_path.string().c_str(), module_path.string().length(),
		                                     function_path.c_str(), function_path.length(),
		                                     params_types, nullptr,
		                                     3, 0,
		                                     &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(p_testmap_set[0] != nullptr);
		REQUIRE(p_testmap_set[1] != nullptr);
		
		auto insert = [](void** p_testmap_set, metaffi_handle hthis, const std::string& key, std::vector<int>* val, int runtime_id)
		{
			cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(3, 0);
			metaffi::runtime::cdts_wrapper wrapper(cdts_param_ret[0].pcdt, cdts_param_ret[0].len);
			wrapper.set(0, {hthis, PYTHON311_RUNTIME_ID, nullptr});
			wrapper.set(1, key);
			wrapper.set(2, {val, (uint64_t)runtime_id, nullptr});

			uint64_t long_err_len = 0;
			char* err = nullptr;
			void(*p)(void*,cdts*,char**,uint64_t*) = ((void(*)(void*,cdts*,char**,uint64_t*))p_testmap_set[0]);
			p(p_testmap_set[1], (cdts*)cdts_param_ret, &err, &long_err_len);
			if(err){ FAIL(err); }
			REQUIRE(long_err_len == 0);
		};

		std::vector<int> vec_to_insert = {1,2,3};
		std::vector<int> vec_to_insert2 = {11,12,13};
		insert(p_testmap_set, testmap_instance, std::string("key1"), &vec_to_insert, 733);
		insert(p_testmap_set, testmap_instance, std::string("key2"), &vec_to_insert2, 733);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
		
		// contains
		function_path = "callable=testmap.contains,instance_required";
		metaffi_type_info params_contains_types[] = {{metaffi_handle_type}, {metaffi_string8_type}};
		metaffi_type_info retvals_contains_types[] = {{metaffi_bool_type}};
		
		void** p_testmap_contains = load_function(module_path.string().c_str(), module_path.string().length(),
		                                          function_path.c_str(), function_path.length(),
		                                          params_contains_types, retvals_contains_types,
		                                          2, 1,
		                                          &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(p_testmap_contains[0] != nullptr);
		REQUIRE(p_testmap_contains[1] != nullptr);
		
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(2, 1);
		metaffi::runtime::cdts_wrapper wrapper_contains_params(cdts_param_ret[0].pcdt, cdts_param_ret[0].len);
		wrapper_contains_params.set(0, {testmap_instance, PYTHON311_RUNTIME_ID, nullptr});
		wrapper_contains_params.set(1, std::string("key1"));
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))p_testmap_contains[0])(p_testmap_contains[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_contains_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_contains_ret[0]->type == metaffi_bool_type);
		REQUIRE(wrapper_contains_ret[0]->cdt_val.metaffi_bool_val.val != 0);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
		
		// get
		function_path = "callable=testmap.get,instance_required";
		metaffi_type_info params_get_types[] = {{metaffi_handle_type}, {metaffi_string8_type}};
		metaffi_type_info retvals_get_types[] = {{metaffi_any_type}};
		
		void** p_testmap_get = load_function(module_path.string().c_str(), module_path.string().length(),
		                                     function_path.c_str(), function_path.length(),
		                                     params_get_types, retvals_get_types,
		                                     2, 1,
		                                     &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(p_testmap_get[0] != nullptr);
		REQUIRE(p_testmap_get[1] != nullptr);

		auto get = [&p_testmap_get](metaffi_handle hthis, const std::string& key) -> std::vector<int>
		{
			cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(2, 1);
			metaffi::runtime::cdts_wrapper wrapper_get_params(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
			wrapper_get_params[0]->type = metaffi_handle_type;
			wrapper_get_params[0]->cdt_val.metaffi_handle_val.val = hthis;
			wrapper_get_params[0]->cdt_val.metaffi_handle_val.runtime_id = PYTHON311_RUNTIME_ID;
			wrapper_get_params[1]->type = metaffi_string8_type;
			wrapper_get_params[1]->cdt_val.metaffi_string8_val.val = (char*)key.c_str();
			wrapper_get_params[1]->cdt_val.metaffi_string8_val.length = key.length();

			uint64_t long_err_len = 0;
			char* err = nullptr;
			((void(*)(void*,cdts*,char**,uint64_t*))p_testmap_get[0])(p_testmap_get[1], (cdts*)cdts_param_ret, &err, &long_err_len);
			if(err){ FAIL(err); }
			REQUIRE(long_err_len == 0);

			metaffi::runtime::cdts_wrapper wrapper_get_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
			REQUIRE(wrapper_get_ret[0]->type == metaffi_handle_type);
			REQUIRE(wrapper_get_ret[0]->cdt_val.metaffi_handle_val.runtime_id == 733);
			std::vector<int>* vector_pulled = (std::vector<int>*)wrapper_get_ret[0]->cdt_val.metaffi_handle_val.val;
			return *vector_pulled;
		};

		std::vector<int> inner_vec1 = get(testmap_instance, std::string("key1"));
		REQUIRE(inner_vec1[0] == 1);
		REQUIRE(inner_vec1[1] == 2);
		REQUIRE(inner_vec1[2] == 3);

		std::vector<int> inner_vec2 = get(testmap_instance, std::string("key2"));
		REQUIRE(inner_vec2[0] == 11);
		REQUIRE(inner_vec2[1] == 12);
		REQUIRE(inner_vec2[2] == 13);

		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
	}
	
	SECTION("runtime_test_target.testmap.get_set_name")
	{
		// load constructor
		std::string function_path = "callable=testmap";
		metaffi_type_info retvals_types[] = {metaffi::runtime::make_type_with_options(metaffi_handle_type)};
		
		void** pnew_testmap = load_function(module_path.string().c_str(), module_path.string().length(),
		                                    function_path.c_str(), function_path.length(),
		                                    nullptr, retvals_types,
		                                    0, 1,
		                                    &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pnew_testmap[0] != nullptr);
		REQUIRE(pnew_testmap[1] != nullptr);
		
		
		// load getter
		function_path = "attribute=name,instance_required,getter";
		metaffi_type_info params_types[2] = {0};
		params_types[0].type = metaffi_handle_type;
		retvals_types[0].type = metaffi_string8_type;
		
		void** pget_name = load_function(module_path.string().c_str(), module_path.string().length(),
		                                    function_path.c_str(), function_path.length(),
		                                    nullptr, retvals_types,
		                                    1, 1,
		                                    &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pget_name[0] != nullptr);
		REQUIRE(pget_name[1] != nullptr);
		
		// load setter
		function_path = "attribute=name,instance_required,setter";
		params_types[0].type = metaffi_handle_type;
		retvals_types[0].type = metaffi_string8_type;
		
		void** pset_name = load_function(module_path.string().c_str(), module_path.string().length(),
		                                 function_path.c_str(), function_path.length(),
		                                 params_types, retvals_types,
		                                 2, 0,
		                                 &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pset_name[0] != nullptr);
		REQUIRE(pset_name[1] != nullptr);
		
		// create new testmap
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pnew_testmap[0])(pnew_testmap[1], (cdts*)cdts_param_ret, &err, &long_err_len);

		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_ret[0]->type == metaffi_handle_type);
		REQUIRE(wrapper_ret[0]->cdt_val.metaffi_handle_val.val != nullptr);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
		
		metaffi_handle testmap_instance = wrapper_ret[0]->cdt_val.metaffi_handle_val.val;
		
		
		// get name
		
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(1, 1);
		metaffi::runtime::cdts_wrapper wrapper_get_params(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper_get_params[0]->type = metaffi_handle_type;
		wrapper_get_params[0]->cdt_val.metaffi_handle_val.val = testmap_instance;
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pget_name[0])(pget_name[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_get_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_get_ret[0]->type == metaffi_string8_type);
		REQUIRE(std::string(wrapper_get_ret[0]->cdt_val.metaffi_string8_val.val, wrapper_get_ret[0]->cdt_val.metaffi_string8_val.length) == "name1");
		
		// set name to "name is my name"
		
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(2, 0);
		metaffi::runtime::cdts_wrapper wrapper_set_params(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper_set_params[0]->type = metaffi_handle_type;
		wrapper_set_params[0]->cdt_val.metaffi_handle_val.val = testmap_instance;
		wrapper_set_params[1]->type = metaffi_string8_type;
		wrapper_set_params[1]->cdt_val.metaffi_string8_val.val = (char*)"name is my name";
		wrapper_set_params[1]->cdt_val.metaffi_string8_val.length = strlen("name is my name");
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pset_name[0])(pset_name[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		// get name again and make sure it is "name is my name"
		cdts* last_get_params = (cdts*)xllr_alloc_cdts_buffer(1, 1);
		wrapper_get_params = metaffi::runtime::cdts_wrapper(last_get_params[0].pcdt, last_get_params[0].len, false);
		wrapper_get_params[0]->type = metaffi_handle_type;
		wrapper_get_params[0]->cdt_val.metaffi_handle_val.val = testmap_instance;
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pget_name[0])(pget_name[1], (cdts*)last_get_params, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper last_get_wrapper(last_get_params[1].pcdt, last_get_params[1].len, false);
		REQUIRE(wrapper_get_ret[0]->type == metaffi_string8_type);
		REQUIRE(std::string(wrapper_get_ret[0]->cdt_val.metaffi_string8_val.val, last_get_wrapper[0]->cdt_val.metaffi_string8_val.length) == "name is my name");
	}
	
	SECTION("runtime_test_target.wait_a_bit")
	{
		// get five_seconds global
		metaffi_type_info var_type[] = {metaffi::runtime::make_type_with_options(metaffi_int64_type)};
		std::string variable_path = "attribute=five_seconds,getter";
		void** pfive_seconds_getter = load_function(module_path.string().c_str(), module_path.string().length(),
		                                            variable_path.c_str(), variable_path.length(),
		                                            nullptr, var_type,
		                                            0, 1,
		                                            &err, &err_len);
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pfive_seconds_getter[0] != nullptr);
		REQUIRE(pfive_seconds_getter[1] != nullptr);
		
		cdts* getter_ret = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pfive_seconds_getter[0])(pfive_seconds_getter[1], (cdts*)getter_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		
		REQUIRE(getter_ret->pcdt->type == metaffi_int64_type);
		REQUIRE(getter_ret->pcdt->cdt_val.metaffi_int64_val.val == 5);
		
		int64_t five = getter_ret->pcdt->cdt_val.metaffi_int64_val.val;
		
		// call wait_a_bit
		std::string function_path = "callable=wait_a_bit";
		metaffi_type_info params_types[] = {metaffi::runtime::make_type_with_options(metaffi_int64_type)};
		
		void** pwait_a_bit = load_function(module_path.string().c_str(), module_path.string().length(),
		                                   function_path.c_str(), function_path.length(),
		                                   params_types, nullptr,
		                                   1, 0,
		                                   &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pwait_a_bit[0] != nullptr);
		REQUIRE(pwait_a_bit[1] != nullptr);
		
		
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(1, 0);
		metaffi::runtime::cdts_wrapper wrapper(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper[0]->type = metaffi_int64_type;
		wrapper[0]->cdt_val.metaffi_int64_val.val = five;
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pwait_a_bit[0])(pwait_a_bit[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		if(cdts_param_ret[0].len + cdts_param_ret[1].len > cdts_cache_size){
			free(cdts_param_ret);
		}
	}
	
	
	SECTION("runtime_test_target.SomeClass")
	{
		std::string function_path = "callable=get_some_classes";
		metaffi_type_info retvals_getSomeClasses_types[] = {{metaffi_handle_array_type, nullptr, 0, 1}};
		
		void** pgetSomeClasses = load_function(module_path.string().c_str(), module_path.string().length(),
		                                       function_path.c_str(), function_path.length(),
		                                       nullptr, retvals_getSomeClasses_types,
		                                       0, 1,
		                                       &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pgetSomeClasses[0] != nullptr);
		REQUIRE(pgetSomeClasses[1] != nullptr);
		
		function_path = "callable=expect_three_some_classes";
		metaffi_type_info params_expectThreeSomeClasses_types[] = {{metaffi_handle_array_type, nullptr, 0, 1}};
		
		void** pexpectThreeSomeClasses = load_function(module_path.string().c_str(), module_path.string().length(),
		                                               function_path.c_str(), function_path.length(),
		                                               params_expectThreeSomeClasses_types, nullptr,
		                                               1, 0,
		                                               &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pexpectThreeSomeClasses[0] != nullptr);
		REQUIRE(pexpectThreeSomeClasses[1] != nullptr);
		
		function_path = "callable=SomeClass.print";
		metaffi_type_info params_SomeClassPrint_types[] = {{metaffi_handle_type, nullptr, 0, 0}};
		
		void** pSomeClassPrint = load_function(module_path.string().c_str(), module_path.string().length(),
		                                       function_path.c_str(), function_path.length(),
		                                       params_SomeClassPrint_types, nullptr,
		                                       1, 0,
		                                       &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pSomeClassPrint[0] != nullptr);
		REQUIRE(pSomeClassPrint[1] != nullptr);
		
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pgetSomeClasses[0])(pgetSomeClasses[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_get_ret(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		REQUIRE(wrapper_get_ret[0]->type == metaffi_handle_array_type);
		REQUIRE(wrapper_get_ret[0]->cdt_val.metaffi_handle_array_val.dimensions == 1);
		REQUIRE(wrapper_get_ret[0]->cdt_val.metaffi_handle_array_val.dimensions_lengths[0] == 3);
		
		auto arr = wrapper_get_ret[0]->cdt_val.metaffi_handle_array_val;
		
		//--------------------------------------------------------------------
		
		cdts* cdts_param_ret_print = (cdts*)xllr_alloc_cdts_buffer(1, 0);
		metaffi::runtime::cdts_wrapper wrapper_print(cdts_param_ret_print[0].pcdt, cdts_param_ret_print[0].len, false);
		wrapper_print[0]->type = metaffi_handle_type;
		wrapper_print[0]->cdt_val.metaffi_handle_val = wrapper_get_ret[0]->cdt_val.metaffi_handle_array_val.vals[1]; // use the 2nd instance
		
		((void(*)(void*,cdts*,char**,uint64_t*))pSomeClassPrint[0])(pSomeClassPrint[1], (cdts*)cdts_param_ret_print, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		//--------------------------------------------------------------------
		
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(1, 0);
		metaffi::runtime::cdts_wrapper wrapper_expect(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper_get_ret[0]->type = metaffi_handle_array_type;
		wrapper_get_ret[0]->cdt_val.metaffi_handle_array_val = arr;
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pexpectThreeSomeClasses[0])(pexpectThreeSomeClasses[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
	}
	
	
	SECTION("runtime_test_target.ThreeBuffers")
	{
		std::string function_path = "callable=expect_three_buffers";
		metaffi_type_info params_expectThreeBuffers_types[] = {{metaffi_uint8_array_type, nullptr, 0, 2}};
		
		void** pexpectThreeBuffers = load_function(module_path.string().c_str(), module_path.string().length(),
		                                           function_path.c_str(), function_path.length(),
		                                           params_expectThreeBuffers_types, nullptr,
		                                           1, 0,
		                                           &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pexpectThreeBuffers[0] != nullptr);
		REQUIRE(pexpectThreeBuffers[1] != nullptr);
		
		function_path = "callable=get_three_buffers";
		metaffi_type_info retval_getThreeBuffers_types[] = {{metaffi_uint8_array_type, nullptr, 0, 2}};
		
		void** pgetThreeBuffers = load_function(module_path.string().c_str(), module_path.string().length(),
		                                        function_path.c_str(), function_path.length(),
		                                        nullptr, retval_getThreeBuffers_types,
		                                        0, 1,
		                                        &err, &err_len);
		
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pgetThreeBuffers[0] != nullptr);
		REQUIRE(pgetThreeBuffers[1] != nullptr);
		
		// pass 3 buffers
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(1, 0);
		metaffi::runtime::cdts_wrapper wrapper_get_ret(cdts_param_ret[0].pcdt, cdts_param_ret[0].len, false);
		wrapper_get_ret[0]->type = metaffi_uint8_array_type;
		wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.dimensions = 2;
		
		wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.dimensions_lengths = (metaffi_size*)malloc(2 * sizeof(metaffi_size));
		wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.dimensions_lengths[0] = 3;
		wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.dimensions_lengths[1] = 3;
		
		wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals = (metaffi_uint8*)malloc(3 * sizeof(metaffi_uint8*));
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[0] = (metaffi_uint8*)malloc(3 * sizeof(metaffi_uint8));
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[1] = (metaffi_uint8*)malloc(3 * sizeof(metaffi_uint8));
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[2] = (metaffi_uint8*)malloc(3 * sizeof(metaffi_uint8));
		
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[0][0] = 0;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[0][1] = 1;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[0][2] = 2;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[1][0] = 3;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[1][1] = 4;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[1][2] = 5;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[2][0] = 6;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[2][1] = 7;
		((metaffi_uint8**)wrapper_get_ret[0]->cdt_val.metaffi_uint8_array_val.vals)[2][2] = 8;
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pexpectThreeBuffers[0])(pexpectThreeBuffers[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		// get 3 buffers
		cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		((void(*)(void*,cdts*,char**,uint64_t*))pgetThreeBuffers[0])(pgetThreeBuffers[1], (cdts*)cdts_param_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		REQUIRE(long_err_len == 0);
		
		metaffi::runtime::cdts_wrapper wrapper_get_buffers(cdts_param_ret[1].pcdt, cdts_param_ret[1].len, false);
		
		REQUIRE(wrapper_get_buffers[0]->type == metaffi_uint8_array_type);
		REQUIRE(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.dimensions == 2);
		REQUIRE(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.dimensions_lengths[0] == 3);
		REQUIRE(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.dimensions_lengths[1] == 3);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[0])[0] == 1);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[0])[1] == 2);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[0])[2] == 3);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[1])[0] == 1);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[1])[1] == 2);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[1])[2] == 3);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[2])[0] == 1);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[2])[1] == 2);
		REQUIRE((((metaffi_uint8**)(wrapper_get_buffers[0]->cdt_val.metaffi_uint8_array_val.vals))[2])[2] == 3);
	}
	
	SECTION("runtime_test_target.set_five_seconds")
	{
		// set five_seconds global
		metaffi_type_info var_type[] = {metaffi::runtime::make_type_with_options(metaffi_int64_type)};
		std::string variable_path = "attribute=five_seconds,setter";
		void** pfive_seconds_setter = load_function(module_path.string().c_str(), module_path.string().length(),
		                                            variable_path.c_str(), variable_path.length(),
		                                            nullptr, var_type,
		                                            1, 0,
		                                            &err, &err_len);
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pfive_seconds_setter[0] != nullptr);
		REQUIRE(pfive_seconds_setter[1] != nullptr);
		
		cdts* setter_ret = (cdts*)xllr_alloc_cdts_buffer(1, 0);
		metaffi::runtime::cdts_wrapper set_wrapper(setter_ret[0].pcdt, setter_ret[0].len, false);
		set_wrapper[0]->type = metaffi_int64_type;
		set_wrapper[0]->cdt_val.metaffi_int64_val.val = 10;
		
		
		uint64_t long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pfive_seconds_setter[0])(pfive_seconds_setter[1], (cdts*)setter_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		
		
		// get five_seconds global
		metaffi_type_info get_var_type[] = {metaffi::runtime::make_type_with_options(metaffi_int64_type)};
		std::string getter_variable_path = "attribute=five_seconds,getter";
		void** pfive_seconds_getter = load_function(module_path.string().c_str(), module_path.string().length(),
		                                            getter_variable_path.c_str(), getter_variable_path.length(),
		                                            nullptr, get_var_type,
		                                            0, 1,
		                                            &err, &err_len);
		if(err){ FAIL(err); }
		REQUIRE(err_len == 0);
		REQUIRE(pfive_seconds_getter[0] != nullptr);
		REQUIRE(pfive_seconds_getter[1] != nullptr);
		
		cdts* getter_ret = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		
		long_err_len = 0;
		((void(*)(void*,cdts*,char**,uint64_t*))pfive_seconds_getter[0])(pfive_seconds_getter[1], (cdts*)getter_ret, &err, &long_err_len);
		if(err){ FAIL(err); }
		
		REQUIRE(getter_ret->pcdt->type == metaffi_int64_type);
		REQUIRE(getter_ret->pcdt->cdt_val.metaffi_int64_val.val == 10);
		
	}
}