#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression):
	expression(std::move(expression))
{

}

ast::ReturnStatement::ReturnStatement()
{

}

std::string ast::ReturnStatement::toString(unsigned int indent) const
{
	return std::string(indent, '\t') + "return" + (expression ? ' ' + expression->toString(indent) : "") + ";\n";
}
