#ifndef FIRM_INTERFACE_HPP
#define FIRM_INTERFACE_HPP

#include <iostream>
#include <libfirm/firm.h>
#include <libfirm/irdump.h>
#include <libfirm/irverify.h>

class FirmInterface
{
	public:
		FirmInterface();
		~FirmInterface();
		void foo();
};

#endif