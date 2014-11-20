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
