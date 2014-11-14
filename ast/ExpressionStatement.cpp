#include "ExpressionStatement.hpp"

ast::ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression>& expr) : expression(std::move(expr))
{

}

std::string ast::ExpressionStatement::toString() const
{
	return expression->toString();
}
