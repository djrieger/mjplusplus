#include "../globals.hpp"
#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(shptr<Expression> expression):
	expression(expression)
{

}

ast::ReturnStatement::ReturnStatement()
{

}

void ast::ReturnStatement::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t') << "return";

	if (expression)
	{
		out << ' ';
		expression->toString(out, indent, true);
	}

	out << ";\n";
}

void ast::ReturnStatement::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto ret_type = Symbol::makeSymbol("return", symbolTable->getCurrentScope())->getCurrentDefinition()->getType();
	ast::Type expr_type = ast::Type(ast::Type::Primitive_type::VOID);

	if (expression)
	{
		if (*ret_type == expr_type)
			sa.printError("Method returns void, but return statement has expression.");
		else
		{
			//TODO: expr_type = expression->getType();
			if (*ret_type != expr_type)
				sa.printError("Mismatched types in return: " + ret_type->getName() + " and " + expr_type.getName());
		}
	}
	else if (*ret_type != expr_type)
		sa.printError("Method returns " + ret_type->getName() + ", but return statement misses expression");
}
