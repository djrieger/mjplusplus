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
#include "../ast/BinaryExpression.hpp"

class FirmInterface
{
	public:
		static FirmInterface& getInstance()
		{
			static FirmInterface instance;
			return instance;
		}
		ir_node* createNodeForIntegerConstant(int);
		ir_node* createNodeForBoolConstant(bool);
	private:
		FirmInterface();
		FirmInterface(FirmInterface const&) = delete;
		void operator=(FirmInterface const&) = delete;
		static shptr<FirmInterface> instance;
	public:
		~FirmInterface();
		void foo();
		ir_mode* getIntegerMode();
		ir_mode* getBooleanMode();
		ir_mode* getReferenceMode();
		ir_node* createOperation(shptr<ast::be::Plus const> expr, ir_node* left, ir_node* right);
};

#endif
