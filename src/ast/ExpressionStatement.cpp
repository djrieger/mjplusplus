#include "../globals.hpp"
#include "ExpressionStatement.hpp"

ast::ExpressionStatement::ExpressionStatement(shptr<Expression> expr) : expression(expr)
{

}

void ast::ExpressionStatement::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	expression->toString(out, indent, true);
	out << ";\n";
}

bool ast::ExpressionStatement::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	expression->get_type(sa, symbolTable);

	if (!expression->standalone())
		sa.printError("Not a statement");

	return false;
}
