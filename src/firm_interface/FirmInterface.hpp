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
		shptr<FirmInterface> instance;
	public:
		static shptr<FirmInterface> getInstance();
		void foo();
		ir_graph* generateMethod(ir_type* owner, shptr<ast::MethodDeclaration const> methodDeclaration);
		ir_graph* generateClass(ir_type* owner, shptr<ast::ClassDeclaration const> classDeclaration);
		ir_type* int_type;
};

#endif