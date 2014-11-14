#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression):
	expression(std::move(expression))
{

}

ast::ReturnStatement::ReturnStatement()
{

}

std::string ast::ReturnStatement::toString() const
{
	return "return" + (expression ? ' ' + expression->toString() : "") + ";\n";
}
