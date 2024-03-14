#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <runtime/runtime_plugin_api.h>
#include <filesystem>
#include <runtime/cdts_wrapper.h>
#include "runtime_id.h"
#include "cdts_python3.h"
#include "py_list.h"
#include "py_float.h"
#include "py_bytes.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif


TEST_CASE( "CDTS Python3 Test", "[cdtspython3]" )
{
	char* err = nullptr;
	uint32_t err_len = 0;
	
	load_runtime(&err, &err_len);
	if(err){
		FAIL(err);
	}
	REQUIRE(err_len == 0);
	
	
	SECTION("Traverse 2D bytes array")
	{
		pyscope();
		
		cdt cdt;
		cdt.type = metaffi_uint8_array_type;
		cdt.cdt_val.metaffi_uint8_array_val.dimension = 2;
		cdt.cdt_val.metaffi_uint8_array_val.length = 3;
		cdt.cdt_val.metaffi_uint8_array_val.arr = new cdt_metaffi_uint8_array[3];
		
		cdt.cdt_val.metaffi_uint8_array_val.arr[0].dimension = 1;
		cdt.cdt_val.metaffi_uint8_array_val.arr[0].length = 2;
		cdt.cdt_val.metaffi_uint8_array_val.arr[0].vals = new metaffi_uint8[2];
		cdt.cdt_val.metaffi_uint8_array_val.arr[0].vals[0] = 1;
		cdt.cdt_val.metaffi_uint8_array_val.arr[0].vals[1] = 2;
		
		cdt.cdt_val.metaffi_uint8_array_val.arr[1].dimension = 1;
		cdt.cdt_val.metaffi_uint8_array_val.arr[1].length = 3;
		cdt.cdt_val.metaffi_uint8_array_val.arr[1].vals = new metaffi_uint8[3];
		cdt.cdt_val.metaffi_uint8_array_val.arr[1].vals[0] = 3;
		cdt.cdt_val.metaffi_uint8_array_val.arr[1].vals[1] = 4;
		cdt.cdt_val.metaffi_uint8_array_val.arr[1].vals[2] = 5;
		
		cdt.cdt_val.metaffi_uint8_array_val.arr[2].dimension = 1;
		cdt.cdt_val.metaffi_uint8_array_val.arr[2].length = 4;
		cdt.cdt_val.metaffi_uint8_array_val.arr[2].vals = new metaffi_uint8[4];
		cdt.cdt_val.metaffi_uint8_array_val.arr[2].vals[0] = 6;
		cdt.cdt_val.metaffi_uint8_array_val.arr[2].vals[1] = 7;
		cdt.cdt_val.metaffi_uint8_array_val.arr[2].vals[2] = 8;
		cdt.cdt_val.metaffi_uint8_array_val.arr[2].vals[3] = 9;
		
		uint8_t arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
		int x = PyGILState_Check();
		PyObject* test0 = PyBytes_FromStringAndSize((char*)arr, 9);
		int y = PyGILState_Check();
		PyObject* test5 = PyBytes_FromStringAndSize((char*)arr, 9);
		int z = PyGILState_Check();
		PyObject* test = PyBytes_FromStringAndSize((char*)arr, 9);
		
		cdts_python3 cdts_py(&cdt, 1);
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
	
	
	SECTION("Traverse 1D bytes array")
	{
		pyscope();
		
		cdt cdt;
		cdt.type = metaffi_uint8_array_type;
		cdt.cdt_val.metaffi_uint8_array_val.dimension = 1;
		cdt.cdt_val.metaffi_uint8_array_val.vals = new metaffi_uint8[3]{1, 2, 3};
		cdt.cdt_val.metaffi_uint8_array_val.length = 3;
		
		cdts_python3 cdts_py(&cdt, 1);
		py_tuple t = cdts_py.to_py_tuple();
		py_bytes bytes(t[0]);
		REQUIRE(bytes.size() == 3);
		
		py_int b1(bytes[0]);
		py_int b2(bytes[1]);
		py_int b3(bytes[2]);
		
		REQUIRE((metaffi_uint8)b1 == 1);
		REQUIRE((metaffi_uint8)b2 == 2);
		REQUIRE((metaffi_uint8)b3 == 3);
		
	}
	
	
	
	SECTION("Traverse 1D array")
	{
		pyscope();
		
		cdt cdt;
		cdt.type = metaffi_float32_array_type;
		cdt.cdt_val.metaffi_float32_array_val.dimension = 1;
		cdt.cdt_val.metaffi_float32_array_val.vals = new metaffi_float32[3]{1.0f, 2.0f, 3.0f};
		cdt.cdt_val.metaffi_float32_array_val.length = 3;
		
		cdts_python3 cdts_py(&cdt, 1);
		py_tuple t = cdts_py.to_py_tuple();
		py_list lst(t[0]);
		REQUIRE(lst.length() == 3);
		
		py_float f1(lst[0]);
		py_float f2(lst[1]);
		py_float f3(lst[2]);
		
		REQUIRE((metaffi_float32)f1 == 1.0f);
		REQUIRE((metaffi_float32)f2 == 2.0f);
		REQUIRE((metaffi_float32)f3 == 3.0f);
	}
	
	SECTION("Traverse 3D array")
	{
		pyscope();
		
		cdt cdt;
		cdt.type = metaffi_float32_array_type;
		cdt.cdt_val.metaffi_float32_array_val.dimension = 3;
		cdt.cdt_val.metaffi_float32_array_val.arr = new cdt_metaffi_float32_array[2]{};
		cdt.cdt_val.metaffi_float32_array_val.length = 2;
		
		cdt.cdt_val.metaffi_float32_array_val.arr[0].dimension = 2;
		cdt.cdt_val.metaffi_float32_array_val.arr[0].length = 2;
		cdt.cdt_val.metaffi_float32_array_val.arr[0].arr = new cdt_metaffi_float32_array[2]{};
		cdt.cdt_val.metaffi_float32_array_val.arr[1].dimension = 2;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].length = 3;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr = new cdt_metaffi_float32_array[3]{};
		
		cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].dimension = 1;
		cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].length = 4;
		cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].vals = new metaffi_float32[4]{1.0f, 2.0f, 3.0f, 4.0f};
		
		cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[1].dimension = 1;
		cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[1].length = 2;
		cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[1].vals = new metaffi_float32[2]{5.0f, 6.0f};
		
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].dimension = 1;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].length = 3;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].vals = new metaffi_float32[3]{7.0f, 8.0f, 9.0f};
		
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[1].dimension = 1;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[1].length = 2;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[1].vals = new metaffi_float32[2]{10.0f, 11.0f};
		
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[2].dimension = 1;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[2].length = 1;
		cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[2].vals = new metaffi_float32[1]{12.0f};
		
		cdts_python3 cdts_py(&cdt, 1);
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
		
		REQUIRE((metaffi_float32)py_float(lst1dim00[0]) == 1.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim00[1]) == 2.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim00[2]) == 3.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim00[3]) == 4.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim01[0]) == 5.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim01[1]) == 6.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim10[0]) == 7.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim10[1]) == 8.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim10[2]) == 9.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim11[0]) == 10.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim11[1]) == 11.0f);
		REQUIRE((metaffi_float32)py_float(lst1dim12[0]) == 12.0f);
	}
	
	SECTION("Construct 1D bytes array")
	{
		pyscope();
		
		uint8_t arr[] = {1, 2, 3};
		py_bytes bytes((const char*)arr, 3);
		
		cdt cdt;
		cdt.type = metaffi_uint8_array_type;
		
		cdts_python3 cdts(&cdt, 1);
		
		metaffi_type_info info = metaffi_type_info{metaffi_uint8_array_type, nullptr, 0, 1};
		cdts.to_cdts((PyObject*)bytes, &info, 1);
		
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.dimension == 1);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.length == 3);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.vals[0] == 1);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.vals[1] == 2);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.vals[2] == 3);
	}
	
	SECTION("Construct 2D bytes array")
	{
		pyscope();
		
		uint8_t arr1[] = {1, 2, 3};
		py_bytes bytes1((const char*)arr1, 3);
		uint8_t arr2[] = {4, 5, 6};
		py_bytes bytes2((const char*)arr2, 3);
		
		py_list lst;
		lst.append((PyObject*)bytes1);
		lst.append((PyObject*)bytes2);
		
		cdt cdt;
		cdt.type = metaffi_uint8_array_type;
		
		cdts_python3 cdts(&cdt, 1);
		
		metaffi_type_info info = metaffi_type_info{metaffi_uint8_array_type, nullptr, 0, 2};
		cdts.to_cdts((PyObject*)lst, &info, 1);
		
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.dimension == 2);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.length == 2);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[0].length == 3);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[0].vals[0] == 1);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[0].vals[1] == 2);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[0].vals[2] == 3);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[1].length == 3);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[1].vals[0] == 4);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[1].vals[1] == 5);
		REQUIRE(cdt.cdt_val.metaffi_uint8_array_val.arr[1].vals[2] == 6);
		
	}
	
	SECTION("Construct 1D array")
	{
		pyscope();
		
		py_list lst;
		lst.append((PyObject*)py_float(1.0f));
		lst.append((PyObject*)py_float(2.0f));
		lst.append((PyObject*)py_float(3.0f));
		
		cdt cdt;
		cdt.type = metaffi_float32_array_type;
		
		cdts_python3 cdts(&cdt, 1);
		
		metaffi_type_info info = metaffi_type_info{metaffi_float32_array_type, nullptr, 0, 1};
		cdts.to_cdts((PyObject*)lst, &info, 1);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.dimension == 1);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.length == 3);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.vals[0] == 1.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.vals[1] == 2.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.vals[2] == 3.0f);
	}
	
	SECTION("Construct 3D array")
	{
		pyscope();
		
		py_list lst3d;
		py_list lst2d0;
		py_list lst2d1;
		py_list lst1d00;
		py_list lst1d01;
		py_list lst1d10;
		py_list lst1d11;
		py_list lst1d12;
		
		lst1d00.append((PyObject*)py_float(1.0f));
		lst1d00.append((PyObject*)py_float(2.0f));
		lst1d00.append((PyObject*)py_float(3.0f));
		lst1d00.append((PyObject*)py_float(4.0f));
		
		lst1d01.append((PyObject*)py_float(5.0f));
		lst1d01.append((PyObject*)py_float(6.0f));
		
		lst1d10.append((PyObject*)py_float(7.0f));
		lst1d10.append((PyObject*)py_float(8.0f));
		lst1d10.append((PyObject*)py_float(9.0f));
		
		lst1d11.append((PyObject*)py_float(10.0f));
		lst1d11.append((PyObject*)py_float(11.0f));
		
		lst1d12.append((PyObject*)py_float(12.0f));
		
		lst2d0.append((PyObject*)lst1d00);
		lst2d0.append((PyObject*)lst1d01);
		
		lst2d1.append((PyObject*)lst1d10);
		lst2d1.append((PyObject*)lst1d11);
		lst2d1.append((PyObject*)lst1d12);
		
		lst3d.append((PyObject*)lst2d0);
		lst3d.append((PyObject*)lst2d1);
		
		cdt cdt;
		cdt.type = metaffi_float32_array_type;
		
		cdts_python3 cdts(&cdt, 1);
		
		metaffi_type_info info = metaffi_type_info{metaffi_float32_array_type, nullptr, 0, 3};
		cdts.to_cdts((PyObject*)lst3d, &info, 1);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.dimension == 3);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.length == 2);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].dimension == 2);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].length == 2);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].dimension == 1);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].length == 4);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].vals[0] == 1.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].vals[1] == 2.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].vals[2] == 3.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[0].vals[3] == 4.0f);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[1].dimension == 1);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[1].length == 2);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[1].vals[0] == 5.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[0].arr[1].vals[1] == 6.0f);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].dimension == 2);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].length == 3);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].dimension == 1);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].length == 3);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].vals[0] == 7.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].vals[1] == 8.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[0].vals[2] == 9.0f);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[1].dimension == 1);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[1].length == 2);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[1].vals[0] == 10.0f);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[1].vals[1] == 11.0f);
		
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[2].dimension == 1);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[2].length == 1);
		REQUIRE(cdt.cdt_val.metaffi_float32_array_val.arr[1].arr[2].vals[0] == 12.0f);
	}
}