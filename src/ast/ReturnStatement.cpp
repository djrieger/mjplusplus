#include "../globals.hpp"
#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(uptr<Expression> expression):
	expression(std::move(expression))
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
