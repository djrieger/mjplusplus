#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(Expression& expression):
	expression(expression)
{

}

std::string ast::ReturnStatement::toString()
{
	return "return " + expression.toString() + ";\n";
}
