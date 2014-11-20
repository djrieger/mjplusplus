#include "../globals.hpp"
#include "ExpressionStatement.hpp"

ast::ExpressionStatement::ExpressionStatement(shptr<Expression> expr) : expression(std::move(expr))
{

}

void ast::ExpressionStatement::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	expression->toString(out, indent, true);
	out << ";\n";
}
