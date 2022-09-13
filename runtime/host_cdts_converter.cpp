#include "host_cdts_converter.h"
#include "cdts_python3.h"
#include "utils.h"
#include <runtime/cdt_capi_loader.h>

//--------------------------------------------------------------------
[[maybe_unused]] cdts* convert_host_params_to_cdts(PyObject* params, PyObject* params_types, metaffi_size return_values_size)
{
	try
	{
		pyscope();
		
#ifdef _DEBUG
		if(!PyTuple_Check(params))
		{
			PyErr_SetString(PyExc_ValueError, "params is not a tuple as expected");
			return nullptr;
		}
		
		if(!PyTuple_Check(params_types))
		{
			PyErr_SetString(PyExc_ValueError, "params_types is not a tuple as expected");
			return nullptr;
		}
#endif
		Py_ssize_t params_size = PyTuple_Size(params);
		if(params_size == 0)
		{
			if(return_values_size == 0){
				return nullptr;
			}
			else{
				return xllr_alloc_cdts_buffer(0, return_values_size);
			}
		}
		
		// get the data from the local objects
		cdts* cdts_buf = xllr_alloc_cdts_buffer(PyTuple_Size(params), return_values_size);
		cdts_python3 pycdts(cdts_buf[0].pcdt, cdts_buf[0].len );
		pycdts.build(params, params_types, 0);
		
		return cdts_buf;
	}
	catch(std::exception& e)
	{
		printf("Failed convert_host_params_to_cdts: %s\n", e.what());
		return nullptr;
	}
}
//--------------------------------------------------------------------
[[maybe_unused]] PyObject* convert_host_return_values_from_cdts(cdts* pcdts, int index)
{
	pyscope();
	cdts_python3 cdts(pcdts[index].pcdt, pcdts[index].len);
	PyObject* o = cdts.parse();
	return o;
}
//--------------------------------------------------------------------