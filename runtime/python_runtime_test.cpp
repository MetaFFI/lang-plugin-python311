#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "cdts_python3.h"
#include "py_bytes.h"
#include "py_float.h"
#include "py_list.h"
#include "py_tuple.h"
#include "runtime_id.h"
#include "utils.h"
#include <filesystem>
#include <runtime/cdts_wrapper.h>
#include <runtime/runtime_plugin_api.h>

#ifdef _DEBUG
#undef _DEBUG

#include <Python.h>

#define _DEBUG
#else
#include <Python.h>
#endif

struct GlobalSetup {
	std::string runtime_test_target_path;

	GlobalSetup()
	{
		if(std::getenv("METAFFI_HOME") == nullptr)
		{
			std::cerr << "METAFFI_HOME is not set. Please set it to the root of the Metaffi project" << std::endl;
			exit(1);
		}

		std::filesystem::path runtime_test_target_std_path(__FILE__);
		runtime_test_target_std_path = runtime_test_target_std_path.parent_path();
		runtime_test_target_std_path.append("./test/runtime_test_target.py");
		runtime_test_target_path = runtime_test_target_std_path.string();

		char* err = nullptr;
		uint32_t err_len = 0;
		load_runtime(&err, &err_len);

		if(err)
		{
			std::cerr << "load runtime failed with " << err << std::endl;
			exit(2);
		}

		if(err_len != 0)
		{
			std::cerr << "Error length is not 0. Expected to be 0" << std::endl;
			exit(3);
		}
	}

	~GlobalSetup()
	{
		char* err = nullptr;
		uint32_t err_len = 0;
		free_runtime(&err, &err_len);

		if(err)
		{
			std::cerr << "free runtime failed with " << err << std::endl;
			exit(4);
		}

		if(err_len != 0)
		{
			std::cerr << "Error length is not 0. Expected to be 0" << std::endl;
			exit(5);
		}
	}
};

static GlobalSetup setup;

char* err = nullptr;
uint32_t err_len = 0;
uint64_t long_err_len = 0;


TEST_SUITE("CDTS Python3 Test")
{

	TEST_CASE("Traverse 2D bytes array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(3, 2, metaffi_uint8_type);
		cdt& cdt_array2d = pcdts[0];
		cdt_array2d.cdt_val.array_val[0] = cdt(2, 1, metaffi_uint8_type);
		cdt_array2d.cdt_val.array_val[1] = cdt(3, 1, metaffi_uint8_type);
		cdt_array2d.cdt_val.array_val[2] = cdt(4, 1, metaffi_uint8_type);

		cdt_array2d.cdt_val.array_val.arr[0].cdt_val.array_val[0] = cdt((metaffi_uint8) 1);
		cdt_array2d.cdt_val.array_val.arr[0].cdt_val.array_val[1] = cdt((metaffi_uint8) 2);

		cdt_array2d.cdt_val.array_val.arr[1].cdt_val.array_val[0] = cdt((metaffi_uint8) 3);
		cdt_array2d.cdt_val.array_val.arr[1].cdt_val.array_val[1] = cdt((metaffi_uint8) 4);
		cdt_array2d.cdt_val.array_val.arr[1].cdt_val.array_val[2] = cdt((metaffi_uint8) 5);

		cdt_array2d.cdt_val.array_val.arr[2].cdt_val.array_val[0] = cdt((metaffi_uint8) 6);
		cdt_array2d.cdt_val.array_val.arr[2].cdt_val.array_val[1] = cdt((metaffi_uint8) 7);
		cdt_array2d.cdt_val.array_val.arr[2].cdt_val.array_val[2] = cdt((metaffi_uint8) 8);
		cdt_array2d.cdt_val.array_val.arr[2].cdt_val.array_val[3] = cdt((metaffi_uint8) 9);

		cdts_python3 cdts_py(pcdts);

		py_tuple t = cdts_py.to_py_tuple();
		py_list lst(t[0]);

		REQUIRE(lst.length() == 3);

		py_bytes b0(lst[0]);
		py_bytes b1(lst[1]);
		py_bytes b2(lst[2]);

		REQUIRE(b0.size() == 2);
		REQUIRE(b0[0] == 1);
		REQUIRE(b0[1] == 2);

		REQUIRE(b1.size() == 3);
		REQUIRE(b1[0] == 3);
		REQUIRE(b1[1] == 4);

		REQUIRE(b2.size() == 4);
		REQUIRE(b2[0] == 6);
		REQUIRE(b2[1] == 7);
		REQUIRE(b2[2] == 8);
		REQUIRE(b2[3] == 9);
	}


	TEST_CASE("Traverse 1D bytes array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(3, 1, metaffi_uint8_type);
		cdt& cdt_array1d = pcdts[0];
		cdt_array1d.cdt_val.array_val[0].cdt_val.uint8_val = 1;
		cdt_array1d.cdt_val.array_val[1].cdt_val.uint8_val = 2;
		cdt_array1d.cdt_val.array_val[2].cdt_val.uint8_val = 3;

		cdts_python3 cdts_py(pcdts);
		py_tuple t = cdts_py.to_py_tuple();
		py_bytes bytes(t[0]);
		REQUIRE(bytes.size() == 3);

		py_int b1(bytes[0]);
		py_int b2(bytes[1]);
		py_int b3(bytes[2]);

		REQUIRE((metaffi_uint8) b1 == 1);
		REQUIRE((metaffi_uint8) b2 == 2);
		REQUIRE((metaffi_uint8) b3 == 3);
	}


	TEST_CASE("Traverse 1D array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(3, 1, metaffi_float32_type);
		cdt& cdt_array1d = pcdts[0];
		cdt_array1d.cdt_val.array_val[0] = cdt((metaffi_float32) 1.0f);
		cdt_array1d.cdt_val.array_val[1] = cdt((metaffi_float32) 2.0f);
		cdt_array1d.cdt_val.array_val[2] = cdt((metaffi_float32) 3.0f);

		cdts_python3 cdts_py(pcdts);
		py_tuple t = cdts_py.to_py_tuple();
		py_list lst(t[0]);
		REQUIRE(lst.length() == 3);

		py_float f1(lst[0]);
		py_float f2(lst[1]);
		py_float f3(lst[2]);

		REQUIRE((metaffi_float32) f1 == 1.0f);
		REQUIRE((metaffi_float32) f2 == 2.0f);
		REQUIRE((metaffi_float32) f3 == 3.0f);
	}

	TEST_CASE("Traverse 3D array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(2, 3, metaffi_float32_type);
		cdt& cdt_array3d = pcdts[0];
		cdt_array3d.cdt_val.array_val[0] = cdt(2, 2, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[1] = cdt(3, 2, metaffi_float32_type);

		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0] = cdt(4, 1, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[1] = cdt(2, 1, metaffi_float32_type);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0] = cdt(3, 1, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[1] = cdt(2, 1, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[2] = cdt(1, 1, metaffi_float32_type);

		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[0] = cdt((metaffi_float32) 1.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[1] = cdt((metaffi_float32) 2.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[2] = cdt((metaffi_float32) 3.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[3] = cdt((metaffi_float32) 4.0f);

		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[1].cdt_val.array_val[0] = cdt((metaffi_float32) 5.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[1].cdt_val.array_val[1] = cdt((metaffi_float32) 6.0f);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0].cdt_val.array_val[0] = cdt((metaffi_float32) 7.0f);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0].cdt_val.array_val[1] = cdt((metaffi_float32) 8.0f);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0].cdt_val.array_val[2] = cdt((metaffi_float32) 9.0f);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[1].cdt_val.array_val[0] = cdt((metaffi_float32) 10.0f);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[1].cdt_val.array_val[1] = cdt((metaffi_float32) 11.0f);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[2].cdt_val.array_val[0] = cdt((metaffi_float32) 12.0f);

		cdts_python3 cdts_py(pcdts);
		py_tuple t = cdts_py.to_py_tuple();
		py_list lst3dim(t[0]);

		REQUIRE(lst3dim.length() == 2);

		py_list lst2dim0(lst3dim[0]);
		py_list lst2dim1(lst3dim[1]);

		REQUIRE(lst2dim0.length() == 2);
		REQUIRE(lst2dim1.length() == 3);

		py_list lst1dim00(lst2dim0[0]);
		py_list lst1dim01(lst2dim0[1]);

		py_list lst1dim10(lst2dim1[0]);
		py_list lst1dim11(lst2dim1[1]);
		py_list lst1dim12(lst2dim1[2]);

		REQUIRE(lst1dim00.length() == 4);
		REQUIRE(lst1dim01.length() == 2);
		REQUIRE(lst1dim10.length() == 3);
		REQUIRE(lst1dim11.length() == 2);
		REQUIRE(lst1dim12.length() == 1);

		REQUIRE((metaffi_float32) py_float(lst1dim00[0]) == 1.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim00[1]) == 2.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim00[2]) == 3.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim00[3]) == 4.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim01[0]) == 5.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim01[1]) == 6.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim10[0]) == 7.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim10[1]) == 8.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim10[2]) == 9.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim11[0]) == 10.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim11[1]) == 11.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim12[0]) == 12.0f);
	}

	TEST_CASE("Construct 1D bytes array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(3, 1, metaffi_uint8_type);
		cdt& cdt_array1d = pcdts[0];
		cdt_array1d.cdt_val.array_val[0] = cdt((metaffi_uint8) 1);
		cdt_array1d.cdt_val.array_val[1] = cdt((metaffi_uint8) 2);
		cdt_array1d.cdt_val.array_val[2] = cdt((metaffi_uint8) 3);

		cdts_python3 cdts_py(pcdts);
		py_tuple t = cdts_py.to_py_tuple();
		py_bytes bytes(t[0]);
		REQUIRE(bytes.size() == 3);

		py_int b1(bytes[0]);
		py_int b2(bytes[1]);
		py_int b3(bytes[2]);

		REQUIRE((metaffi_uint8) b1 == 1);
		REQUIRE((metaffi_uint8) b2 == 2);
		REQUIRE((metaffi_uint8) b3 == 3);
	}

	TEST_CASE("Construct 2D bytes array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(2, 2, metaffi_uint8_type);
		cdt& cdt_array2d = pcdts[0];
		cdt_array2d.cdt_val.array_val[0] = cdt(3, 1, metaffi_uint8_type);
		cdt_array2d.cdt_val.array_val[1] = cdt(3, 1, metaffi_uint8_type);

		cdt_array2d.cdt_val.array_val[0].cdt_val.array_val[0] = cdt((metaffi_uint8) 1);
		cdt_array2d.cdt_val.array_val[0].cdt_val.array_val[1] = cdt((metaffi_uint8) 2);
		cdt_array2d.cdt_val.array_val[0].cdt_val.array_val[2] = cdt((metaffi_uint8) 3);

		cdt_array2d.cdt_val.array_val[1].cdt_val.array_val[0] = cdt((metaffi_uint8) 4);
		cdt_array2d.cdt_val.array_val[1].cdt_val.array_val[1] = cdt((metaffi_uint8) 5);
		cdt_array2d.cdt_val.array_val[1].cdt_val.array_val[2] = cdt((metaffi_uint8) 6);

		cdts_python3 cdts_py(pcdts);
		py_tuple t = cdts_py.to_py_tuple();
		py_list lst(t[0]);
		REQUIRE(lst.length() == 2);

		py_bytes bytes1(lst[0]);
		py_bytes bytes2(lst[1]);

		REQUIRE(bytes1.size() == 3);
		REQUIRE(bytes2.size() == 3);

		py_int b11(bytes1[0]);
		py_int b12(bytes1[1]);
		py_int b13(bytes1[2]);

		py_int b21(bytes2[0]);
		py_int b22(bytes2[1]);
		py_int b23(bytes2[2]);

		REQUIRE((metaffi_uint8) b11 == 1);
		REQUIRE((metaffi_uint8) b12 == 2);
		REQUIRE((metaffi_uint8) b13 == 3);

		REQUIRE((metaffi_uint8) b21 == 4);
		REQUIRE((metaffi_uint8) b22 == 5);
		REQUIRE((metaffi_uint8) b23 == 6);
	}

	TEST_CASE("Construct 1D array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(3, 1, metaffi_float32_type);
		cdt& cdt_array1d = pcdts[0];
		cdt_array1d.cdt_val.array_val[0] = cdt((metaffi_float32) 1.0f);
		cdt_array1d.cdt_val.array_val[1] = cdt((metaffi_float32) 2.0f);
		cdt_array1d.cdt_val.array_val[2] = cdt((metaffi_float32) 3.0f);

		cdts_python3 cdts_py(pcdts);
		py_tuple t = cdts_py.to_py_tuple();
		py_list lst(t[0]);
		REQUIRE(lst.length() == 3);

		py_float f1(lst[0]);
		py_float f2(lst[1]);
		py_float f3(lst[2]);

		REQUIRE((metaffi_float32) f1 == 1.0f);
		REQUIRE((metaffi_float32) f2 == 2.0f);
		REQUIRE((metaffi_float32) f3 == 3.0f);
	}

	TEST_CASE("Construct 3D array")
	{
		pyscope();

		cdts pcdts(1, 1);
		pcdts[0] = cdt(2, 3, metaffi_float32_type);
		cdt& cdt_array3d = pcdts[0];
		cdt_array3d.cdt_val.array_val[0] = cdt(2, 2, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[1] = cdt(3, 2, metaffi_float32_type);

		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0] = cdt(4, 1, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[1] = cdt(2, 1, metaffi_float32_type);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0] = cdt(3, 1, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[1] = cdt(2, 1, metaffi_float32_type);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[2] = cdt(1, 1, metaffi_float32_type);

		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[0] = cdt((metaffi_float32) 1.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[1] = cdt((metaffi_float32) 2.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[2] = cdt((metaffi_float32) 3.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[0].cdt_val.array_val[3] = cdt((metaffi_float32) 4.0f);

		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[1].cdt_val.array_val[0] = cdt((metaffi_float32) 5.0f);
		cdt_array3d.cdt_val.array_val[0].cdt_val.array_val[1].cdt_val.array_val[1] = cdt((metaffi_float32) 6.0f);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0].cdt_val.array_val[0] = cdt((metaffi_float32) 7.0f);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0].cdt_val.array_val[1] = cdt((metaffi_float32) 8.0f);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[0].cdt_val.array_val[2] = cdt((metaffi_float32) 9.0f);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[1].cdt_val.array_val[0] = cdt((metaffi_float32) 10.0f);
		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[1].cdt_val.array_val[1] = cdt((metaffi_float32) 11.0f);

		cdt_array3d.cdt_val.array_val[1].cdt_val.array_val[2].cdt_val.array_val[0] = cdt((metaffi_float32) 12.0f);

		cdts_python3 cdts_py(pcdts);
		py_tuple t = cdts_py.to_py_tuple();
		py_list lst3dim(t[0]);

		REQUIRE(lst3dim.length() == 2);

		py_list lst2dim0(lst3dim[0]);
		py_list lst2dim1(lst3dim[1]);

		REQUIRE(lst2dim0.length() == 2);
		REQUIRE(lst2dim1.length() == 3);

		py_list lst1dim00(lst2dim0[0]);
		py_list lst1dim01(lst2dim0[1]);

		py_list lst1dim10(lst2dim1[0]);
		py_list lst1dim11(lst2dim1[1]);
		py_list lst1dim12(lst2dim1[2]);

		REQUIRE(lst1dim00.length() == 4);
		REQUIRE(lst1dim01.length() == 2);
		REQUIRE(lst1dim10.length() == 3);
		REQUIRE(lst1dim11.length() == 2);
		REQUIRE(lst1dim12.length() == 1);

		REQUIRE((metaffi_float32) py_float(lst1dim00[0]) == 1.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim00[1]) == 2.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim00[2]) == 3.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim00[3]) == 4.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim01[0]) == 5.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim01[1]) == 6.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim10[0]) == 7.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim10[1]) == 8.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim10[2]) == 9.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim11[0]) == 10.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim11[1]) == 11.0f);
		REQUIRE((metaffi_float32) py_float(lst1dim12[0]) == 12.0f);
	}
}

void** cppload_function(const std::string& module_path,
                        const std::string& function_path,
                        std::vector<metaffi_type_info> params_types,
                        std::vector<metaffi_type_info> retvals_types)
{
	err = nullptr;
	uint32_t err_len_load = 0;

	metaffi_type_info* params_types_arr = params_types.empty() ? nullptr : params_types.data();
	metaffi_type_info* retvals_types_arr = retvals_types.empty() ? nullptr : retvals_types.data();

	void** pfunction = load_function(module_path.c_str(), module_path.length(),
	                                 function_path.c_str(), function_path.length(),
	                                 params_types_arr, retvals_types_arr,
	                                 params_types.size(), retvals_types.size(),
	                                 &err, &err_len_load);

	if(err)
	{
		FAIL(std::string(err));
	}
	REQUIRE(err_len_load == 0);
	REQUIRE(pfunction[0] != nullptr);
	REQUIRE(pfunction[1] != nullptr);

	return pfunction;
};


TEST_SUITE("Python Runtime Tests")
{

	TEST_CASE("runtime_test_target.hello_world")
	{
		std::string function_path = "callable=hello_world";
		void** phello_world = cppload_function(setup.runtime_test_target_path, function_path, {}, {});

		((void (*)(void*, char**, uint64_t*)) phello_world[0])(phello_world[1], &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
	}

	TEST_CASE("runtime_test_target.returns_an_error")
	{
		std::string function_path = "callable=returns_an_error";
		void** preturns_an_error = cppload_function(setup.runtime_test_target_path, function_path, {}, {});

		((void (*)(void*, char**, uint64_t*)) preturns_an_error[0])(preturns_an_error[1], &err, &long_err_len);
		REQUIRE(err != nullptr);
		REQUIRE(long_err_len > 0);

		err = nullptr;
		long_err_len = 0;
	}

	TEST_CASE("runtime_test_target.div_integers")
	{
		std::string function_path = "callable=div_integers";
		std::vector<metaffi_type_info> params_types = {metaffi_type_info{metaffi_int64_type},
		                                               metaffi_type_info{metaffi_int64_type}};
		std::vector<metaffi_type_info> retvals_types = {metaffi_type_info{metaffi_float64_type}};

		void** pdiv_integers = cppload_function(setup.runtime_test_target_path, function_path, params_types,
		                                        retvals_types);

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(params_types.size(), retvals_types.size());
		cdts& params = cdts_param_ret[0];
		cdts& ret = cdts_param_ret[1];

		params[0] = cdt((metaffi_int64) 10);
		params[1] = cdt((metaffi_int64) 2);

		((void (*)(void*, cdts*, char**, uint64_t*)) pdiv_integers[0])(pdiv_integers[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(ret[0].type == metaffi_float64_type);
		REQUIRE(ret[0].cdt_val.float64_val == 5.0);
	}

	TEST_CASE("runtime_test_target.join_strings")
	{
		std::string function_path = "callable=join_strings";
		std::vector<metaffi_type_info> params_types = {metaffi_type_info{metaffi_string8_array_type}};
		std::vector<metaffi_type_info> retvals_types = {metaffi_type_info{metaffi_string8_type}};

		void** join_strings = cppload_function(setup.runtime_test_target_path, function_path, params_types,
		                                       retvals_types);

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(params_types.size(), retvals_types.size());
		cdts& params = cdts_param_ret[0];
		cdts& ret = cdts_param_ret[1];

		params[0] = cdt(3, 1, metaffi_string8_type);
		params[0].cdt_val.array_val[0] = cdt((metaffi_string8) "one", true);
		params[0].cdt_val.array_val[1] = cdt((metaffi_string8) "two", true);
		params[0].cdt_val.array_val[2] = cdt((metaffi_string8) "three", true);

		((void (*)(void*, cdts*, char**, uint64_t*)) join_strings[0])(join_strings[1], (cdts*) cdts_param_ret, &err,
		                                                              &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(ret[0].type == metaffi_string8_type);
		REQUIRE(std::u8string(ret[0].cdt_val.string8_val) == u8"one,two,three");
	}

	TEST_CASE("runtime_test_target.wait_a_bit")
	{
		// get five_seconds global
		std::vector<metaffi_type_info> var_type = {metaffi_type_info{metaffi_int64_type}};
		std::string variable_path = "attribute=five_seconds,getter";
		void** pfive_seconds_getter = cppload_function(setup.runtime_test_target_path, variable_path, {}, var_type);

		cdts* getter_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		cdts& ret = getter_ret[1];

		((void (*)(void*, cdts*, char**, uint64_t*)) pfive_seconds_getter[0])(pfive_seconds_getter[1],
		                                                                      (cdts*) getter_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(ret[0].type == metaffi_int64_type);
		REQUIRE(ret[0].cdt_val.int64_val == 5);

		int64_t five = ret[0].cdt_val.int64_val;

		// call wait_a_bit
		std::string function_path = "callable=wait_a_bit";
		std::vector<metaffi_type_info> params_types = {metaffi_type_info{metaffi_int64_type}};

		void** pwait_a_bit = cppload_function(setup.runtime_test_target_path, function_path, params_types, {});

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(params_types.size(), 0);
		cdts& params = cdts_param_ret[0];

		params[0] = cdt((metaffi_int64) five);

		((void (*)(void*, cdts*, char**, uint64_t*)) pwait_a_bit[0])(pwait_a_bit[1], (cdts*) cdts_param_ret, &err,
		                                                             &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
	}

	TEST_CASE("runtime_test_target.testmap.set_get_contains")
	{
		// create new testmap
		std::string function_path = "callable=testmap";
		std::vector<metaffi_type_info> retvals_types = {metaffi_type_info(metaffi_handle_type)};

		void** pnew_testmap = cppload_function(setup.runtime_test_target_path, function_path, {}, retvals_types);

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		cdts& ret = cdts_param_ret[1];

		((void (*)(void*, cdts*, char**, uint64_t*)) pnew_testmap[0])(pnew_testmap[1], (cdts*) cdts_param_ret, &err,
		                                                              &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(ret[0].type == metaffi_handle_type);
		REQUIRE(ret[0].cdt_val.handle_val.val != nullptr);
		REQUIRE(ret[0].cdt_val.handle_val.runtime_id == PYTHON311_RUNTIME_ID);

		cdt_metaffi_handle testmap_instance = ret[0].cdt_val.handle_val;

		// set
		function_path = "callable=testmap.set,instance_required";
		std::vector<metaffi_type_info> params_types = {metaffi_type_info(metaffi_handle_type),
		                                               metaffi_type_info(metaffi_string8_type),
		                                               metaffi_type_info(metaffi_any_type)};

		void** p_testmap_set = cppload_function(setup.runtime_test_target_path, function_path, params_types, {});

		cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(params_types.size(), 0);
		cdts& params = cdts_param_ret[0];

		params[0] = cdt(testmap_instance);
		params[1] = cdt((metaffi_string8) u8"key", true);
		params[2] = cdt((metaffi_int64) 42);

		((void (*)(void*, cdts*, char**, uint64_t*)) p_testmap_set[0])(p_testmap_set[1], (cdts*) cdts_param_ret, &err,
		                                                               &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		// contains
		function_path = "callable=testmap.contains,instance_required";
		params_types = {metaffi_type_info(metaffi_handle_type),
		                metaffi_type_info(metaffi_string8_type)};
		retvals_types = {metaffi_type_info(metaffi_bool_type)};

		void** p_testmap_contains = cppload_function(setup.runtime_test_target_path, function_path, params_types,
		                                             retvals_types);

		cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(params_types.size(), retvals_types.size());
		cdts& contains_params = cdts_param_ret[0];
		cdts& contains_ret = cdts_param_ret[1];

		contains_params[0] = cdt(testmap_instance);
		contains_params[1] = cdt((metaffi_string8) u8"key", true);

		((void (*)(void*, cdts*, char**, uint64_t*)) p_testmap_contains[0])(p_testmap_contains[1],
		                                                                    (cdts*) cdts_param_ret, &err,
		                                                                    &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(contains_ret[0].type == metaffi_bool_type);
		REQUIRE(contains_ret[0].cdt_val.bool_val != 0);

		// get
		function_path = "callable=testmap.get,instance_required";
		params_types = {metaffi_type_info(metaffi_handle_type),
		                metaffi_type_info(metaffi_string8_type)};
		retvals_types = {metaffi_type_info(metaffi_any_type)};

		void** p_testmap_get = cppload_function(setup.runtime_test_target_path, function_path, params_types,
		                                        retvals_types);

		cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(params_types.size(), retvals_types.size());
		cdts& get_params = cdts_param_ret[0];
		cdts& get_ret = cdts_param_ret[1];

		get_params[0] = cdt(testmap_instance);
		get_params[1] = cdt((metaffi_string8) u8"key", true);

		((void (*)(void*, cdts*, char**, uint64_t*)) p_testmap_get[0])(p_testmap_get[1], (cdts*) cdts_param_ret, &err,
		                                                               &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(get_ret[0].type == metaffi_int64_type);
		REQUIRE(get_ret[0].cdt_val.int64_val == 42);
	}

	TEST_CASE("runtime_test_target.testmap.set_get_contains_cpp_object")
	{
		// create new testmap
		std::string function_path = "callable=testmap";
		std::vector<metaffi_type_info> retvals_types = {metaffi_type_info(metaffi_handle_type)};

		void** pnew_testmap = cppload_function(setup.runtime_test_target_path, function_path, {}, retvals_types);

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);

		((void (*)(void*, cdts*, char**, uint64_t*)) pnew_testmap[0])(pnew_testmap[1], (cdts*) cdts_param_ret, &err,
		                                                              &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		cdts& wrapper_ret = cdts_param_ret[1];
		REQUIRE(wrapper_ret[0].type == metaffi_handle_type);
		REQUIRE(wrapper_ret[0].cdt_val.handle_val.val != nullptr);
		REQUIRE(wrapper_ret[0].cdt_val.handle_val.runtime_id == PYTHON311_RUNTIME_ID);

		cdt_metaffi_handle testmap_instance = wrapper_ret[0].cdt_val.handle_val;

		// set
		function_path = "callable=testmap.set,instance_required";
		std::vector<metaffi_type_info> params_types = {metaffi_type_info(metaffi_handle_type),
		                                               metaffi_type_info(metaffi_string8_type),
		                                               metaffi_type_info(metaffi_any_type)};

		void** p_testmap_set = cppload_function(setup.runtime_test_target_path, function_path, params_types, {});

		auto insert = [](void** p_testmap_set, const cdt_metaffi_handle& hthis, const std::u8string& key,
		                 std::vector<int8_t>* val,
		                 int runtime_id) {
			cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(3, 0);
			cdts& cdt_params = cdts_param_ret[0];
			cdt_params[0] = cdt(hthis);
			cdt_params[1] = cdt((metaffi_string8) key.c_str(), true);
			cdt_params[2] = cdt({val, (uint64_t) runtime_id, nullptr});

			uint64_t long_err_len = 0;
			char* err = nullptr;
			void (*p)(void*, cdts*, char**, uint64_t*) = ((void (*)(void*, cdts*, char**, uint64_t*)) p_testmap_set[0]);
			p(p_testmap_set[1], (cdts*) cdts_param_ret, &err, &long_err_len);
			if(err)
			{
				FAIL(std::string(err));
			}
			REQUIRE(long_err_len == 0);
		};

		std::vector<int8_t> obj_to_insert1 = {1, 2, 3};
		std::vector<int8_t> obj_to_insert2 = {11, 12, 13};
		insert(p_testmap_set, testmap_instance, std::u8string(u8"key1"), &obj_to_insert1, 733);
		insert(p_testmap_set, testmap_instance, std::u8string(u8"key2"), &obj_to_insert2, 733);

		// contains
		function_path = "callable=testmap.contains,instance_required";
		std::vector<metaffi_type_info> params_contains_types = {metaffi_type_info(metaffi_handle_type),
		                                                        metaffi_type_info(metaffi_string8_type)};
		std::vector<metaffi_type_info> retvals_contains_types = {metaffi_type_info(metaffi_bool_type)};

		void** p_testmap_contains = cppload_function(setup.runtime_test_target_path, function_path,
		                                             params_contains_types, retvals_contains_types);

		cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(2, 1);
		cdts& params = cdts_param_ret[0];
		params[0] = cdt(testmap_instance);
		params[1] = cdt(u8"key1", true);

		((void (*)(void*, cdts*, char**, uint64_t*)) p_testmap_contains[0])(p_testmap_contains[1],
		                                                                    (cdts*) cdts_param_ret, &err,
		                                                                    &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		cdts& wrapper_contains_ret = cdts_param_ret[1];
		REQUIRE(wrapper_contains_ret[0].type == metaffi_bool_type);
		REQUIRE(wrapper_contains_ret[0].cdt_val.bool_val != 0);

		// get
		function_path = "callable=testmap.get,instance_required";
		std::vector<metaffi_type_info> params_get_types = {metaffi_type_info(metaffi_handle_type),
		                                                   metaffi_type_info(metaffi_string8_type)};
		std::vector<metaffi_type_info> retvals_get_types = {metaffi_type_info(metaffi_any_type)};

		void** p_testmap_get = cppload_function(setup.runtime_test_target_path, function_path, params_get_types,
		                                        retvals_get_types);

		auto get = [&p_testmap_get](const cdt_metaffi_handle& hthis, const std::u8string& key) -> std::vector<int8_t> {
			cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(2, 1);
			cdts& wrapper_get_params = cdts_param_ret[0];
			wrapper_get_params[0] = cdt(hthis);
			wrapper_get_params[1] = cdt((metaffi_string8) key.c_str(), true);

			uint64_t long_err_len = 0;
			char* err = nullptr;
			((void (*)(void*, cdts*, char**, uint64_t*)) p_testmap_get[0])(p_testmap_get[1], (cdts*) cdts_param_ret,
			                                                               &err, &long_err_len);
			if(err)
			{
				FAIL(std::string(err));
			}
			REQUIRE(long_err_len == 0);

			cdts& wrapper_get_ret = cdts_param_ret[1];
			REQUIRE(wrapper_get_ret[0].type == metaffi_handle_type);
			REQUIRE(wrapper_get_ret[0].cdt_val.handle_val.runtime_id == 733);
			std::vector<int8_t>* object_pulled = (std::vector<int8_t>*) wrapper_get_ret[0].cdt_val.handle_val.val;
			return *object_pulled;
		};

		std::vector<int8_t> inner_obj1 = get(testmap_instance, std::u8string(u8"key1"));
		REQUIRE(inner_obj1 == std::vector<int8_t>{1, 2, 3});

		std::vector<int8_t> inner_obj2 = get(testmap_instance, std::u8string(u8"key2"));
		REQUIRE(inner_obj2 == std::vector<int8_t>{11, 12, 13});
	}

	TEST_CASE("runtime_test_target.testmap.get_set_name")
	{
		// Load constructor
		std::string function_path = "callable=testmap";
		std::vector<metaffi_type_info> retvals_types = {metaffi_type_info(metaffi_handle_type)};

		void** pnew_testmap = cppload_function(setup.runtime_test_target_path, function_path, {}, retvals_types);

		// Load getter
		function_path = "attribute=name,instance_required,getter";
		std::vector<metaffi_type_info> params_types = {metaffi_type_info{metaffi_handle_type}, metaffi_type_info{metaffi_string8_type}};

		void** pget_name = cppload_function(setup.runtime_test_target_path, function_path, params_types, retvals_types);

		// Load setter
		function_path = "attribute=name,instance_required,setter";
		params_types = {metaffi_type_info{metaffi_handle_type}, metaffi_type_info{metaffi_string8_type}};

		void** pset_name = cppload_function(setup.runtime_test_target_path, function_path, params_types, {});

		// Create new testmap
		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		uint64_t long_err_len = 0;
		((void (*)(void*, cdts*, char**, uint64_t*)) pnew_testmap[0])(pnew_testmap[1], (cdts*) cdts_param_ret, &err, &long_err_len);

		// Check for errors
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(cdts_param_ret[1].arr[0].type == metaffi_handle_type);
		REQUIRE(cdts_param_ret[1].arr[0].cdt_val.handle_val.val != nullptr);

		cdt_metaffi_handle testmap_instance = cdts_param_ret[1].arr[0].cdt_val.handle_val;

		// Get name
		cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(1, 1);
		cdts_param_ret[0].arr[0] = cdt(testmap_instance);

		long_err_len = 0;
		((void (*)(void*, cdts*, char**, uint64_t*)) pget_name[0])(pget_name[1], (cdts*) cdts_param_ret, &err,
		                                                           &long_err_len);
		// Check for errors
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(cdts_param_ret[1].arr[0].type == metaffi_string8_type);
		REQUIRE(std::u8string(cdts_param_ret[1].arr[0].cdt_val.string8_val) == u8"name1");

		// Set name to "name is my name"
		cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(2, 0);
		cdts_param_ret[0].arr[0] = cdt(testmap_instance);
		cdts_param_ret[0].arr[1] = cdt(u8"name is my name", true);

		long_err_len = 0;
		((void (*)(void*, cdts*, char**, uint64_t*)) pset_name[0])(pset_name[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		// Check for errors
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		// Get name again and make sure it is "name is my name"
		cdts* last_get_params = (cdts*) xllr_alloc_cdts_buffer(1, 1);
		last_get_params[0].arr[0] = cdt(testmap_instance);

		long_err_len = 0;
		((void (*)(void*, cdts*, char**, uint64_t*)) pget_name[0])(pget_name[1], (cdts*) last_get_params, &err, &long_err_len);
		// Check for errors
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(last_get_params[1].arr[0].type == metaffi_string8_type);
		REQUIRE(std::u8string(last_get_params[1].arr[0].cdt_val.string8_val) == u8"name is my name");
	}

	TEST_CASE("runtime_test_target.SomeClass")
	{
		std::string function_path = "callable=get_some_classes";
		std::vector<metaffi_type_info> retvals_getSomeClasses_types = {metaffi_type_info{metaffi_handle_array_type}};

		void** pgetSomeClasses = cppload_function(setup.runtime_test_target_path, function_path, {}, retvals_getSomeClasses_types);

		function_path = "callable=expect_three_some_classes";
		std::vector<metaffi_type_info> params_expectThreeSomeClasses_types = {metaffi_type_info{metaffi_handle_array_type}};

		void** pexpectThreeSomeClasses = cppload_function(setup.runtime_test_target_path, function_path, params_expectThreeSomeClasses_types, {});

		function_path = "callable=SomeClass.print";
		std::vector<metaffi_type_info> params_SomeClassPrint_types = {metaffi_type_info{metaffi_handle_type}};

		void** pSomeClassPrint = cppload_function(setup.runtime_test_target_path, function_path, params_SomeClassPrint_types, {});

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		cdts& res = cdts_param_ret[1];

		((void (*)(void*, cdts*, char**, uint64_t*)) pgetSomeClasses[0])(pgetSomeClasses[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(res[0].type == metaffi_handle_array_type);
		REQUIRE(res[0].cdt_val.array_val.length == 3);
		REQUIRE(res[0].cdt_val.array_val.arr[0].cdt_val.handle_val.val != nullptr);
		REQUIRE(res[0].cdt_val.array_val.arr[1].cdt_val.handle_val.val != nullptr);
		REQUIRE(res[0].cdt_val.array_val.arr[2].cdt_val.handle_val.val != nullptr);

		std::vector<cdt_metaffi_handle> some_classes = {res[0].cdt_val.array_val.arr[0].cdt_val.handle_val,
		                                                res[0].cdt_val.array_val.arr[1].cdt_val.handle_val,
		                                                res[0].cdt_val.array_val.arr[2].cdt_val.handle_val};

		// call print on each SomeClass
		cdts* cdts_param_ret3 = (cdts*) xllr_alloc_cdts_buffer(1, 0);
		cdts& print_params = cdts_param_ret3[0];
		print_params[0] = cdt(some_classes[0]);
		
		((void (*)(void*, cdts*, char**, uint64_t*)) pSomeClassPrint[0])(pSomeClassPrint[1], (cdts*) cdts_param_ret3, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		print_params[0] = cdt(some_classes[1]);
		((void (*)(void*, cdts*, char**, uint64_t*)) pSomeClassPrint[0])(pSomeClassPrint[1], (cdts*) cdts_param_ret3, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		print_params[0] = cdt(some_classes[2]);
		((void (*)(void*, cdts*, char**, uint64_t*)) pSomeClassPrint[0])(pSomeClassPrint[1], (cdts*) cdts_param_ret3, &err,
		                                                                 &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
		
		
		cdts* cdts_param_ret2 = (cdts*) xllr_alloc_cdts_buffer(1, 0);
		cdts& params = cdts_param_ret2[0];
		params[0] = cdt(3, 1, metaffi_handle_type);
		params[0].cdt_val.array_val[0] = cdt(some_classes[0]);
		params[0].cdt_val.array_val[1] = cdt(some_classes[1]);
		params[0].cdt_val.array_val[2] = cdt(some_classes[2]);

		((void (*)(void*, cdts*, char**, uint64_t*)) pexpectThreeSomeClasses[0])(pexpectThreeSomeClasses[1], (cdts*) cdts_param_ret2, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
	}


	TEST_CASE("runtime_test_target.ThreeBuffers")
	{
		std::string function_path = "callable=expect_three_buffers";
		void** pexpectThreeBuffers = cppload_function(setup.runtime_test_target_path, function_path,
		                                              {metaffi_type_info{metaffi_uint8_array_type, nullptr, false, 2}}, {});

		function_path = "callable=get_three_buffers";
		void** pgetThreeBuffers = cppload_function(setup.runtime_test_target_path, function_path, {}, {{metaffi_uint8_array_type, nullptr, false, 2}});

		// pass 3 buffers
		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(1, 0);
		cdts& params = cdts_param_ret[0];
		cdts& ret = cdts_param_ret[1];
		params[0] = cdt(3, 2, metaffi_uint8_array_type);
		params[0].cdt_val.array_val[0] = cdt(2, 1, metaffi_uint8_array_type);
		params[0].cdt_val.array_val[0].cdt_val.array_val.arr[0] = cdt((metaffi_uint8) 1);
		params[0].cdt_val.array_val[0].cdt_val.array_val.arr[1] = cdt((metaffi_uint8) 2);

		params[0].cdt_val.array_val[1] = cdt(3, 1, metaffi_uint8_array_type);
		params[0].cdt_val.array_val[1].cdt_val.array_val.arr[0] = cdt((metaffi_uint8) 3);
		params[0].cdt_val.array_val[1].cdt_val.array_val.arr[1] = cdt((metaffi_uint8) 4);
		params[0].cdt_val.array_val[1].cdt_val.array_val.arr[2] = cdt((metaffi_uint8) 5);

		params[0].cdt_val.array_val[2] = cdt(4, 1, metaffi_uint8_array_type);
		params[0].cdt_val.array_val[2].cdt_val.array_val.arr[0] = cdt((metaffi_uint8) 6);
		params[0].cdt_val.array_val[2].cdt_val.array_val.arr[1] = cdt((metaffi_uint8) 7);
		params[0].cdt_val.array_val[2].cdt_val.array_val.arr[2] = cdt((metaffi_uint8) 8);
		params[0].cdt_val.array_val[2].cdt_val.array_val.arr[3] = cdt((metaffi_uint8) 9);

		((void (*)(void*, cdts*, char**, uint64_t*)) pexpectThreeBuffers[0])(pexpectThreeBuffers[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		// get 3 buffers
		cdts* get_three_buffers_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		ret = std::move(get_three_buffers_ret[1]);
		((void (*)(void*, cdts*, char**, uint64_t*)) pgetThreeBuffers[0])(pgetThreeBuffers[1], get_three_buffers_ret,
		                                                                  &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(ret[0].type == metaffi_uint8_array_type);
		REQUIRE(ret[0].cdt_val.array_val.fixed_dimensions == 2);
		REQUIRE(ret[0].cdt_val.array_val.length == 3);

		REQUIRE(ret[0].cdt_val.array_val.arr[0].cdt_val.array_val.fixed_dimensions == 1);
		REQUIRE(ret[0].cdt_val.array_val.arr[0].cdt_val.array_val.length == 4);
		REQUIRE(ret[0].cdt_val.array_val.arr[0].cdt_val.array_val.arr[0].cdt_val.uint8_val == 1);
		REQUIRE(ret[0].cdt_val.array_val.arr[0].cdt_val.array_val.arr[1].cdt_val.uint8_val == 2);
		REQUIRE(ret[0].cdt_val.array_val.arr[0].cdt_val.array_val.arr[2].cdt_val.uint8_val == 3);
		REQUIRE(ret[0].cdt_val.array_val.arr[0].cdt_val.array_val.arr[3].cdt_val.uint8_val == 4);

		REQUIRE(ret[0].cdt_val.array_val.arr[1].cdt_val.array_val.fixed_dimensions == 1);
		REQUIRE(ret[0].cdt_val.array_val.arr[1].cdt_val.array_val.length == 3);
		REQUIRE(ret[0].cdt_val.array_val.arr[1].cdt_val.array_val.arr[0].cdt_val.uint8_val == 5);
		REQUIRE(ret[0].cdt_val.array_val.arr[1].cdt_val.array_val.arr[1].cdt_val.uint8_val == 6);
		REQUIRE(ret[0].cdt_val.array_val.arr[1].cdt_val.array_val.arr[2].cdt_val.uint8_val == 7);

		REQUIRE(ret[0].cdt_val.array_val.arr[2].cdt_val.array_val.fixed_dimensions == 1);
		REQUIRE(ret[0].cdt_val.array_val.arr[2].cdt_val.array_val.length == 2);
		REQUIRE(ret[0].cdt_val.array_val.arr[2].cdt_val.array_val.arr[0].cdt_val.uint8_val == 8);
		REQUIRE(ret[0].cdt_val.array_val.arr[2].cdt_val.array_val.arr[1].cdt_val.uint8_val == 9);
	}


	TEST_CASE("runtime_test_target.set_five_seconds")
	{
		// set five_seconds global;
		std::string variable_path = "attribute=five_seconds,setter";
		void** pfive_seconds_setter = cppload_function(setup.runtime_test_target_path, variable_path, {metaffi_type_info(metaffi_int64_type)}, {});

		cdts* setter_ret = (cdts*) xllr_alloc_cdts_buffer(1, 0);
		cdts& params = setter_ret[0];
		params[0] = cdt((metaffi_int64) 10);

		((void (*)(void*, cdts*, char**, uint64_t*)) pfive_seconds_setter[0])(pfive_seconds_setter[1],
		                                                                      (cdts*) setter_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}


		// get five_seconds global
		std::string getter_variable_path = "attribute=five_seconds,getter";
		void** pfive_seconds_getter = cppload_function(setup.runtime_test_target_path, getter_variable_path, {}, {metaffi_type_info(metaffi_int64_type)});

		cdts* getter_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		cdts& ret = getter_ret[1];

		((void (*)(void*, cdts*, char**, uint64_t*)) pfive_seconds_getter[0])(pfive_seconds_getter[1],
		                                                                      (cdts*) getter_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}

		REQUIRE(ret[0].type == metaffi_int64_type);
		REQUIRE(ret[0].cdt_val.int64_val == 10);
	}

	TEST_CASE("return_null")
	{
		void** preturn_null = cppload_function(setup.runtime_test_target_path, "callable=return_null", {}, {metaffi_type_info{metaffi_handle_type}});

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		cdts& ret = cdts_param_ret[1];

		((void (*)(void*, cdts*, char**, uint64_t*)) preturn_null[0])(preturn_null[1], (cdts*) cdts_param_ret, &err,
		                                                              &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);

		REQUIRE(ret[0].type == metaffi_null_type);
	}

	TEST_CASE("returns_array_of_different_objects")
	{
		// returns [1, 'string', 3.0, None, bytes([1, 2, 3]), SomeClass()]
		void** preturns_array_of_different_objects = cppload_function(setup.runtime_test_target_path,
		                                                              "callable=returns_array_of_different_objects",
		                                                              {}, {metaffi_type_info{metaffi_any_type}});

		cdts* cdts_param_ret = (cdts*) xllr_alloc_cdts_buffer(0, 1);
		cdts& ret = cdts_param_ret[1];

		((void (*)(void*, cdts*, char**, uint64_t*)) preturns_array_of_different_objects[0])(preturns_array_of_different_objects[1],
		                                                                                     (cdts*) cdts_param_ret, &err, &long_err_len);

		REQUIRE((ret[0].type & metaffi_array_type) != 0);
		REQUIRE(ret[0].cdt_val.array_val.fixed_dimensions == MIXED_OR_UNKNOWN_DIMENSIONS);
		REQUIRE(ret[0].cdt_val.array_val.length == 6);

		REQUIRE(ret[0].cdt_val.array_val.arr[0].type == metaffi_int64_type);
		REQUIRE(ret[0].cdt_val.array_val.arr[1].type == metaffi_string8_type);
		REQUIRE(ret[0].cdt_val.array_val.arr[2].type == metaffi_float64_type);
		REQUIRE(ret[0].cdt_val.array_val.arr[3].type == metaffi_null_type);
		REQUIRE(ret[0].cdt_val.array_val.arr[4].type == metaffi_uint8_array_type);
		REQUIRE(ret[0].cdt_val.array_val.arr[5].type == metaffi_handle_type);

		REQUIRE(ret[0].cdt_val.array_val.arr[0].cdt_val.int64_val == 1);
		REQUIRE(std::u8string(ret[0].cdt_val.array_val.arr[1].cdt_val.string8_val) == u8"string");
		REQUIRE(ret[0].cdt_val.array_val.arr[2].cdt_val.float64_val == 3.0);
		REQUIRE(ret[0].cdt_val.array_val.arr[4].cdt_val.array_val.length == 3);
		REQUIRE(ret[0].cdt_val.array_val.arr[4].cdt_val.array_val.fixed_dimensions == 1);
		REQUIRE(ret[0].cdt_val.array_val.arr[4].cdt_val.array_val.arr[0].cdt_val.uint8_val == 1);
		REQUIRE(ret[0].cdt_val.array_val.arr[4].cdt_val.array_val.arr[1].cdt_val.uint8_val == 2);
		REQUIRE(ret[0].cdt_val.array_val.arr[4].cdt_val.array_val.arr[2].cdt_val.uint8_val == 3);
		REQUIRE(ret[0].cdt_val.array_val.arr[5].cdt_val.handle_val.val != nullptr);
		REQUIRE(((PyObject*) (ret[0].cdt_val.array_val.arr[5].cdt_val.handle_val.val))->ob_refcnt > 0);
	}

	TEST_CASE("call_any")
	{
		// call with 1, 'string', 3.0, null, bytes([1, 2, 3]), SomeClass()
        void** pcall_any = cppload_function(setup.runtime_test_target_path, "callable=accepts_any",
		                                    {metaffi_type_info{metaffi_int64_type}, metaffi_type_info{metaffi_any_type}}, {});
		
		void** pnew_someclass = cppload_function(setup.runtime_test_target_path, "callable=SomeClass",
		                                    {}, {metaffi_type_info(metaffi_handle_type)});
		
		cdts* cdts_param_ret = (cdts*)xllr_alloc_cdts_buffer(2, 0);
		cdts& params = cdts_param_ret[0];
		
		params[0] = cdt((metaffi_int64) 0);
		params[1] = cdt((metaffi_int64) 1);
		((void (*)(void*, cdts*, char**, uint64_t*)) pcall_any[0])(pcall_any[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
		
		params[0] = cdt((metaffi_int64) 1);
		params[1] = cdt((metaffi_string8) u8"string", true);
		((void (*)(void*, cdts*, char**, uint64_t*)) pcall_any[0])(pcall_any[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
		
		params[0] = cdt((metaffi_int64) 2);
		params[1] = cdt((metaffi_float64) 3.0);
		((void (*)(void*, cdts*, char**, uint64_t*)) pcall_any[0])(pcall_any[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
		
		params[0] = cdt((metaffi_int64) 3);
		params[1] = cdt(cdt_metaffi_handle{nullptr, 0, nullptr});
		((void (*)(void*, cdts*, char**, uint64_t*)) pcall_any[0])(pcall_any[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
		
		params[0] = cdt((metaffi_int64) 4);
		params[1] = cdt(1, 1, metaffi_uint8_array_type);
		params[1].cdt_val.array_val.arr[0] = cdt((metaffi_uint8) 1);
		params[1].cdt_val.array_val.arr[1] = cdt((metaffi_uint8) 2);
		params[1].cdt_val.array_val.arr[2] = cdt((metaffi_uint8) 3);
		((void (*)(void*, cdts*, char**, uint64_t*)) pcall_any[0])(pcall_any[1], (cdts*) cdts_param_ret, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
		
		// new some class
		cdts* cdts_param_ret2 = (cdts*)xllr_alloc_cdts_buffer(0, 1);
		cdts& someclass_ret = cdts_param_ret2[1];
		((void (*)(void*, cdts*, char**, uint64_t*)) pnew_someclass[0])(pnew_someclass[1], (cdts*) cdts_param_ret2, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
		REQUIRE(someclass_ret[0].type == metaffi_handle_type);
		
		auto someclass_instance = someclass_ret[0].cdt_val.handle_val;
		
		cdts* cdts_param_ret3 = (cdts*)xllr_alloc_cdts_buffer(2, 0);
		params = std::move(cdts_param_ret3[0]);
		params[0] = cdt((metaffi_int64) 5);
		params[1] = cdt(someclass_instance);
		((void (*)(void*, cdts*, char**, uint64_t*)) pcall_any[0])(pcall_any[1], (cdts*) cdts_param_ret3, &err, &long_err_len);
		if(err)
		{
			FAIL(std::string(err));
		}
		REQUIRE(long_err_len == 0);
	}

	TEST_CASE("!call_callback_add")
	{
	}

	TEST_CASE("!return_multiple_return_values")
	{
	}

	TEST_CASE("!extended_test - test different python calling methods + properties")
	{
	}
}


TEST_SUITE("libraries")
{

	TEST_CASE("!collections.deque")
	{
	}

	TEST_CASE("!3rd party - beautifulsoup4")
	{
	}

	TEST_CASE("!3rd party - numpy")
	{
	}

	TEST_CASE("!3rd party - pandas")
	{
	}
}