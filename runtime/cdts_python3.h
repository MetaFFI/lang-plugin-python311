#pragma once
#include <runtime/cdts_wrapper.h>
#include<unordered_map>
#include <memory>
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#include "py_tuple.h"

#define _DEBUG
#else
#include <Python.h>
#endif



#define build_constructor_param(type) \
	const decltype(set_##type)& set_##type, \
    const decltype(set_##type##_array)& set_##type##_array

#define build_constructor_init_param(type) \
	set_##type(std::move(set_##type)),\
	set_##type##_array(std::move(set_##type##_array))


extern "C" const char* create_lambda_python_code;

class cdts_python3
{
private:
	metaffi::runtime::cdts_wrapper cdts;
	
public:
	explicit cdts_python3(cdt* cdts, metaffi_size cdts_length);
	
	/**
	 * @brief Parses CDTS and returns PyTuple. Method assumes CDTS is filled!
	 * @return PyTuple with CDTS in Python objects
	 */
	py_tuple to_py_tuple();
	
	/**
	 * @brief Builds CDTS from given Tuple with python objects. Function assumes CDTS is empty and in the correct size!
	 * @param pyobject_or_tuple PyTuple to fill CDTS
	 * @param starting_index Starting index in tuple to fill CDTS
	 */
	void to_cdts(PyObject* pyobject_or_tuple, metaffi_type_info* expected_types, int expected_types_length);
	
};
