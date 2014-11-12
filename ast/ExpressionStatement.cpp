#include "ExpressionStatement.hpp"

ast::ExpressionStatement::ExpressionStatement(Expression& expr) : expression(expr)
{

}

std::string ast::ExpressionStatement::toString()
{
	return expression.toString();
}