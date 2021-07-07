#include "host_cdts_converter.h"
#include "cdts_python3.h"

//--------------------------------------------------------------------
cdt* convert_host_params_to_cdts(PyObject* params, PyObject* params_types)
{
	if(!PyTuple_Check(params))
	{
		PyErr_SetString(PyExc_ValueError, "params_names is not a tuple as expected");
		return nullptr;
	}
	
	if(!PyTuple_Check(params_types))
	{
		PyErr_SetString(PyExc_ValueError, "params_types is not a tuple as expected");
		return nullptr;
	}
	
	// get the data from the local objects
	cdts_python3 cdts(PyTuple_Size(params));
	
	cdts.build(params, params_types, 0);
	
	return cdts.get_cdts();
}
//--------------------------------------------------------------------
PyObject* convert_host_return_values_from_cdts(PyObject* return_values_names, cdt* cdt_return_values)
{
	if(!PyTuple_Check(return_values_names))
	{
		PyErr_SetString(PyExc_ValueError, "params_names is not a tuple as expected");
		return nullptr;
	}
	
	Py_ssize_t return_values_count = PyTuple_Size(return_values_names);
	cdts_python3 cdts(cdt_return_values, return_values_count);
	return cdts.parse();
}
//--------------------------------------------------------------------