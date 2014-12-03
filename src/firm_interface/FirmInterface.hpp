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

namespace std
{
	template<>
	struct hash<pair<ir_type*, string>>
	{
		size_t operator ()(pair<ir_type*, string> const& t) const
		{
			hash<ir_type*> hi;
			hash<string> hs;
			return hi(t.first) ^ hs(t.second);
		}
	};
}

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

		std::unordered_map<shptr<ast::Type>, ir_type*> types;
		std::unordered_map<std::pair<ir_type*, std::string>, ir_entity*> classMethodEntities;
		std::unordered_map<std::pair<ir_type*, std::string>, ir_entity*> classFieldEntities;

	public:
		~FirmInterface();
		void foo();
		void convert(shptr<ast::Program> program);
		ir_mode* getIntegerMode();
		ir_mode* getBooleanMode();
		ir_mode* getReferenceMode();
		ir_mode* getMode(shptr<ast::Type> ast_type);
		ir_type* getType(shptr<ast::Type> ast_type);
		void addClassType(shptr<ast::Ident> class_ident, ir_type* class_type);
		void addMethod(ir_type* class_type, std::string method_name, ir_entity* ent);
		void addField(ir_type* class_type, std::string method_name, ir_entity* ent);
		ir_entity* getMethodEntity(ir_type* class_type, std::string method_name);
		ir_entity* getFieldEntity(ir_type* class_type, std::string method_name);
		ir_node* createOperation(shptr<ast::be::Plus const> expr, ir_node* left, ir_node* right);

};

#endif
