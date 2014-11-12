#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(Expression& expr):
	expr(expr)
{

}

std::string ast::ReturnStatement::toString()
{
	return "return " + expression.toString() + ";\n";
}
