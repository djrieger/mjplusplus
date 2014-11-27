#ifndef FIRM_INTERFACE_HPP
#define FIRM_INTERFACE_HPP

#include <iostream>
#include <libfirm/firm.h>
#include <libfirm/irdump.h>
#include <libfirm/irverify.h>

#include "../ast/MethodDeclaration.hpp"
#include "../ast/TypeIdent.hpp"
#include "../ast/Statement.hpp"
#include "../ast/Block.hpp"

class FirmInterface
{
	private:
		FirmInterface();
		~FirmInterface();
		static shptr<FirmInterface> instance;
	public:
		static shptr<FirmInterface> getInstance();
		void foo();
};

#endif