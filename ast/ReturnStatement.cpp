#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression):
	expression(std::move(expression))
{

}

std::string ast::ReturnStatement::toString()
{
	return "return " + expression->toString() + ";\n";
}
