#include "FirmInterface.hpp"
#include "../lexer/token.hpp"
#include <cstring>
#include "../ast/LVDStatement.hpp"
#include "../ast/Block.hpp"
#include "../visitors/MemberVisitor.hpp"


FirmInterface::FirmInterface()
{
	ir_init();
	printf("Initialized libFirm Version: %d.%d\n", ir_get_version_major(), ir_get_version_minor());
}

ir_node* FirmInterface::createNodeForIntegerConstant(int x)
{
	return new_Const_long(mode_Is, x);
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
