#include "ExpressionStatement.hpp"

ast::ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression>& expr) : expression(std::move(expr))
{

}

std::string ast::ExpressionStatement::toString(unsigned int indent) const
{
	return std::string(indent, '\t') + expression->toString(indent) + '\n';
}
