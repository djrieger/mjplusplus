#include "FirmInterface.hpp"

FirmInterface::FirmInterface() 
{
	ir_init();
	printf("Initialized libFirm Version: %d.%d\n",
	       ir_get_version_major(),
	       ir_get_version_minor());
}

FirmInterface::~FirmInterface()
{
	ir_finish();
}