#ifndef FIRM_INTERFACE_HPP
#define FIRM_INTERFACE_HPP

#include <iostream>
#include <libfirm/firm.h>
#include <libfirm/irdump.h>
#include <libfirm/irverify.h>

#include "../ast/MethodDeclaration.hpp"
#include "../ast/TypeIdent.hpp"

class FirmInterface
{
	public:
		FirmInterface();
		~FirmInterface();
		void foo();
		ir_entity* generateVoidMethod(ir_type* owner, shptr<ast::MethodDeclaration> methodDeclaration);

		ir_type* int_type;
};

#endif