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
#include "../ast/MethodInvocation.hpp"

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

struct JumpTarget
{
	ir_node* targetNode;
	bool first;

	JumpTarget(): targetNode(NULL), first(false) {}

	void jumpFromBlock(ir_node* sourceBlock)
	{
		std::cout << "jumpFromBlock ";

		if (!this->targetNode)
		{
			std::cout << " zeroeth ";
			this->targetNode = sourceBlock;
			this->first = true;
		}
		else if (this->first)
		{
			std::cout << " first ";
			ir_node* curBlock = get_cur_block();

			this->targetNode = new_immBlock();
			ir_node* jmpNode = new_r_Jmp(this->targetNode);

			this->first = false;
			add_immBlock_pred(this->targetNode, curBlock);

			set_cur_block(this->targetNode);

			//ir_node* targetBlock = new_immBlock();


		}

		if (this->targetNode)
		{
			std::cout << " second ";
			ir_node* curBlock = get_cur_block();
			std::cout << " 1 ";
			ir_node* jmpNode = new_r_Jmp(sourceBlock);
			std::cout << " 2 ";
			add_immBlock_pred(this->targetNode, curBlock);
			std::cout << " 3 ";
			set_cur_block(this->targetNode);
			//set_cur_block(this->targetNode);
		}

		std::cout << std::endl;
	}
};

class FirmInterface
{

	private:
		FirmInterface();
		FirmInterface(FirmInterface const&) = delete;
		void operator=(FirmInterface const&) = delete;
		static shptr<FirmInterface> instance;
		std::string in_name;
		std::string out_name;
		shptr<std::map<std::string, int>> var2pos;

		std::unordered_map<shptr<ast::Type>, ir_type*> types;
		std::unordered_map<std::pair<ir_type*, std::string>, ir_entity*> classMethodEntities;
		std::unordered_map<std::pair<ir_type*, std::string>, ir_entity*> classFieldEntities;

		ir_node* createNodeForMethodCall(ir_node* caller,
		                                 ir_type* class_type,
		                                 std::string const& method_name,
		                                 shptr<ast::Arguments const> arguments,
		                                 shptr<ast::MethodDeclaration const> methodDeclaration);

	public:
		static FirmInterface& getInstance()
		{
			static FirmInterface instance;
			return instance;
		}
		ir_node* createNodeForMethodCall(shptr<ast::pe::MethodInvocation const> expr);
		ir_node* createNodeForMethodCall(ir_node* caller, shptr<ast::MethodInvocation const> expr);
		ir_node* createNodeForCallocCall(ir_node* count, unsigned int size);
		ir_node* createNodeForIntegerConstant(int64_t);
		ir_node* createNodeForBooleanConstant(bool);
		ir_node* createModOperation(ir_node* left, ir_node* right);
		ir_node* createDivOperation(ir_node* left, ir_node* right);
		ir_node* createNullPointerNode();

		~FirmInterface();
		void foo();
		void convert(shptr<ast::Program> program);
		void build();
		static std::string replace_dollar(std::string name);
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
		ir_entity* createMethodEntity(ir_type* caller, shptr<ast::MethodDeclaration const> methodDeclaration);
		std::tuple<ir_entity*, ir_entity*> getSystemNode();
		void initSystem();

		void setInput(std::string const& in);
		void setOutput(std::string const& out);

		shptr<std::map<std::string, int>> getVarMap();
		void setVarMap(shptr<std::map<std::string, int>> newVar2pos);

};

#endif
