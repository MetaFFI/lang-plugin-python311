#include "metaffi_package_importer.h"
#include <string>
#include "utils.h"
#include <cstdio>

#include "python3_api_wrapper.h"

void import_metaffi_package()
{
	if(pPyErr_Occurred())
	{
		printf("Error before import metaffi script\n");
		pPyErr_Print();
		pPyErr_Clear();
	}

const char* script = R"(
import sys
sys.__loading_within_xllr_python3 = True
from metaffi import *
del sys.__loading_within_xllr_python3
)";

	pPyRun_SimpleString(script);
	if(pPyErr_Occurred())
	{
		printf("Error after import metaffi module!!!!!!!! did you pip install metaffi-api? Error:\n");
		pPyErr_Print();
		pPyErr_Clear();
	}

}