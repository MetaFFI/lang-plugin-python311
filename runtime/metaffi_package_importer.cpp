#include "metaffi_package_importer.h"
#include <string>
#include "utils.h"
#include <cstdio>

#include "python3_api_wrapper.h"

void import_metaffi_package()
{
	if(PyErr_Occurred())
	{
		printf("Error before import metaffi script\n");
		PyErr_Print();
		PyErr_Clear();
	}

const char* script = R"(
from metaffi import *
)";

	PyRun_SimpleString(script);
	if(PyErr_Occurred())
	{
		printf("Error after import metaffi module!!!!!!!! did you pip install metaffi-api? Error:\n");
		PyErr_Print();
		PyErr_Clear();
	}

}