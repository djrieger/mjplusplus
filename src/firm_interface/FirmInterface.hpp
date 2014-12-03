#ifndef FIRM_INTERFACE_HPP
#define FIRM_INTERFACE_HPP

#include <unordered_map>

#include <libfirm/firm.h>
#include <libfirm/irdump.h>
#include <libfirm/irverify.h>

#include "../ast/MethodDeclaration.hpp"
#include "../ast/TypeIdent.hpp"
#include "../ast/Statement.hpp"
#include "../ast/Block.hpp"
#include "../ast/BinaryExpression.hpp"
#include "../ast/PrimaryExpression.hpp"

class FirmInterface
{
	public:
		static FirmInterface& getInstance()
		{
			static FirmInterface instance;
			return instance;
		}
		ir_node* createNodeForMethodCall(shptr<ast::pe::MethodInvocation const> expr);
		ir_node* createNodeForIntegerConstant(int);
		ir_node* createNodeForBoolConstant(bool);
	private:
		FirmInterface();
		FirmInterface(FirmInterface const&) = delete;
		void operator=(FirmInterface const&) = delete;
		static shptr<FirmInterface> instance;
		std::string in_name;
		std::string out_name;

		std::unordered_map<shptr<ast::Type>, ir_type*> types;
	public:
		~FirmInterface();
		void foo();
		void convert(shptr<ast::Program> program);
		void build();
		ir_mode* getIntegerMode();
		ir_mode* getBooleanMode();
		ir_mode* getReferenceMode();
		ir_mode* getMode(shptr<ast::Type> ast_type);
		ir_type* getType(shptr<ast::Type> ast_type);
		void addClassType(shptr<ast::Ident> class_ident, ir_type* class_type);
		ir_node* createOperation(shptr<ast::be::Plus const> expr, ir_node* left, ir_node* right);

		void setInput(std::string const& in);
		void setOutput(std::string const& out);
};

#endif
