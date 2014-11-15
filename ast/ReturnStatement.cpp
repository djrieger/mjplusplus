#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression):
	expression(std::move(expression))
{

}

ast::ReturnStatement::ReturnStatement()
{

}

void ast::ReturnStatement::toString(std::ostream& out, unsigned int indent) const
{
	out << std::string(indent, '\t') << "return";

	if (expression)
	{
		out << ' ';
		expression->toString(out, indent);
	}

	out << ";\n";
}
