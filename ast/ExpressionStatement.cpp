#include "ExpressionStatement.hpp"

ast::ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression>& expr) : expression(std::move(expr))
{

}

void ast::ExpressionStatement::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	expression->toString(out, indent, true);
	out << ";\n";
}
