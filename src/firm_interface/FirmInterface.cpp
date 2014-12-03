#include <iostream>

#include <cstring>

#include "FirmInterface.hpp"

#include "../lexer/token.hpp"
#include "../ast/LVDStatement.hpp"
#include "../ast/Block.hpp"
#include "../visitors/ProgramVisitor.hpp"
#include "../visitors/MemberVisitor.hpp"
#include "../visitors/ExpressionVisitor.hpp"


FirmInterface::FirmInterface()
{
	ir_init();
	printf("Initialized libFirm Version: %d.%d\n", ir_get_version_major(), ir_get_version_minor());
}

void FirmInterface::convert(shptr<ast::Program> program)
{
	std::cout << "converting Program" << std::endl;
	ProgramVisitor v;

	try
	{
		program->accept(v);
	}
	catch (char const* e)
	{
		std::cerr << e << std::endl;
		throw;
	}
}

ir_node* FirmInterface::createNodeForMethodCall(shptr<ast::pe::MethodInvocation const> expr)
{

	// TODO: find the corresponding entity for this method
	ir_type* class_type = NULL;// TODO: get ast::Type of calling object ("this" or whatever)
	std::string const& method_name = expr->getIdentifier();
	ir_entity* ent = getMethodEntity(class_type, method_name);

	shptr<ast::Arguments> arguments = expr->getArguments();
	int argc = arguments->getArgumentsSize();

	ir_node** in = (ir_node**) calloc(argc, sizeof(ir_node*));
	int in_counter = 0;

	ExpressionVisitor exprVisitor;

	//TODO: We also need to add "this" to the parameters.
	for (shptr<ast::Expression> argumentExpr : * (arguments->getArgumentExpressions()))
	{
		argumentExpr->accept(exprVisitor);
		in[in_counter++] = exprVisitor.getResultNode();
	}

	// create the call
	ir_node* store = get_store();
	ir_node* callee = new_Address(ent);
	ir_node* call_node = new_Call(store, callee, argc, in, get_entity_type(ent));

	// update the current store
	ir_node* new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
	set_store(new_store);

	// get the result
	ir_node* tuple = new_Proj(call_node, get_modeT(), pn_Call_T_result);
	ir_node* result = new_Proj(tuple, getIntegerMode(), 0);

	free(in);

	return result;
}

ir_node* FirmInterface::createNodeForIntegerConstant(int x)
{
	return new_Const_long(getIntegerMode(), x);
}

ir_node* FirmInterface::createNodeForBoolConstant(bool x)
{
	//TODO: let this create a real boolean node
	return new_Const_long(getIntegerMode(), (x ? 1 : 0));
}

void FirmInterface::foo()
{
	const unsigned int paramsCount = 0;
	const unsigned int resultsCount = 0;
	const unsigned int localVarsCount = 0;

	ir_type* owner = get_glob_type();      /* the class in which this method is defined */
	auto typeInt = std::make_shared<ast::Type>(ast::Type::Primitive_type::VOID);
	// Token(Token_type const& token_type, std::string const& string_value, source_position_t const& position);
	lexer::Token mainToken(lexer::Token::Token_type::TOKEN_IDENT, "main", std::pair<unsigned int, unsigned int>(1, 1));
	auto ident = std::make_shared<ast::Ident>(mainToken);
	auto typeIdent = std::make_shared<ast::TypeIdent>(typeInt, ident);

	auto parameters = std::make_shared<vec<shptr<ast::TypeIdent>>>();
	lexer::Token paramToken(lexer::Token::Token_type::TOKEN_IDENT, "argc", std::pair<unsigned int, unsigned int>(1, 1));
	auto parameterName = std::make_shared<ast::Ident>(paramToken);
	parameters->push_back(std::make_shared<ast::TypeIdent>(typeInt, parameterName));

	auto block = std::shared_ptr<ast::Block>();
	ir_graph* irg; //= generateMethod(owner, std::make_shared<ast::MethodDeclaration>(typeIdent, parameters, block));

	set_current_ir_graph(irg);

	ir_node* currentMemState = get_store();
	//ir_node *startBlockArray[1];
	//startBlockArray[0] = get_irg_start_block(irg);
	ir_node* returnNode = new_Return (currentMemState, 0, NULL); //1, startBlockArray);
	mature_immBlock (get_r_cur_block(irg));

	add_immBlock_pred (get_irg_end_block(irg), returnNode); // number of inputs does not match metho type (0 inputs, 1 declared)
	mature_immBlock (get_irg_end_block(irg));
	/*
	if (irg_verify(irg) != 0) {
		std::cout << "Verification successful;" << std::endl;
	} else {
		std::cout << "Verification failed;" << std::endl;
	}*/
	irg_finalize_cons (irg);
	dump_ir_graph (irg, 0);

	std::cout << "Dumped ycomp graph" << std::endl;
}

FirmInterface::~FirmInterface()
{
	// TODO: free graph, ir_type nodes etc.
	ir_finish();
	std::cout << "Destroyed FirmInterface instance" << std::endl;
}

ir_mode* FirmInterface::getIntegerMode()
{
	return mode_Is;
}

ir_mode* FirmInterface::getBooleanMode()
{
	return mode_Bu; /* TODO*/
}

ir_mode* FirmInterface::getReferenceMode()
{
	return mode_P;
}

//probably to do: getMode(type) -> mode
ir_mode* FirmInterface::getMode(shptr<ast::Type> ast_type)
{
	if (ast_type->isAnyInteger())
		return getIntegerMode();
	else if (ast_type->isBool())
		return getBooleanMode();
	else
		return getReferenceMode();

}

ir_type* FirmInterface::getType(shptr<ast::Type> ast_type)
{
	auto it = types.find(ast_type);

	if (it != types.end())
		return it->second;

	ir_type* r;

	if (ast_type->isBool() || ast_type->isInteger())
	{
		// add new primitive
		r = new_type_primitive(getMode(ast_type));

	}
	else if (ast_type->isArray())
	{
		// add new array + recursion for lesser dimensions
		r = new_type_array(getType(ast_type->de_array()));
	}
	else
	{
		std::cerr << "Trying to get firm type for ";
		ast_type->toString(std::cerr, 0);
		std::cerr << " which should exist already, returning int pointer." << std::endl;
		ir_type* int_type = getType(std::make_shared<ast::Type>(ast::Type::Primitive_type::INT));
		r = new_type_pointer(int_type);
	}

	types[ast_type] = r;
	return r;
}

void FirmInterface::addClassType(shptr<ast::Ident> class_ident, ir_type* class_type)
{
	auto ast_type = std::make_shared<ast::Type>(class_ident);
	std::cerr << "Adding type for " << get_class_name(class_type) << "." << std::endl;
	auto it = types.find(ast_type);

	if (it != types.end())
	{
		std::cerr << "\taka fixing the existing one" << std::endl;
		set_pointer_points_to_type(it->second, class_type);
	}
	else
		types[ast_type] = new_type_pointer(class_type);
}

void FirmInterface::addMethod(ir_type* class_type, std::string method_name, ir_entity* ent)
{
	classMethodEntities[ {class_type, method_name}] = ent;
}

ir_entity* FirmInterface::getMethodEntity(ir_type* class_type, std::string method_name)
{
	return classMethodEntities[ {class_type, method_name}];
}

void FirmInterface::addField(ir_type* class_type, std::string method_name, ir_entity* ent)
{
	classFieldEntities[ {class_type, method_name}] = ent;
}

ir_entity* FirmInterface::getFieldEntity(ir_type* class_type, std::string method_name)
{
	return classFieldEntities[ {class_type, method_name}];
}

// overload for all
ir_node* FirmInterface::createOperation(shptr<ast::be::Plus const> expr, ir_node* left, ir_node* right)
{
	// first paramater is dbg_info, last parameter is mode
	// probably use new_r_Add(block, ...);
	return new_d_Add(NULL, left, right, getIntegerMode());
}
