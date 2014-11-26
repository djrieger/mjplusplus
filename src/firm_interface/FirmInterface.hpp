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
	public:
		FirmInterface();
		~FirmInterface();
		void foo();
		ir_entity* generateVoidMethod(ir_type* owner, shptr<ast::MethodDeclaration> methodDeclaration);

		ir_type* int_type;

		unsigned int countVariableDeclarations(shptr<ast::Block> functionBody);
};

#endif