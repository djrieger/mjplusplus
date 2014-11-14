#include "WhileStatement.hpp"

ast::WhileStatement::WhileStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& statement) :
	condition(std::move(condition)), statement(std::move(statement))
{

}

std::string ast::WhileStatement::toString(unsigned int indent) const
{
	return std::string(indent, '\t') + "while (" + condition->toString(indent) + ") " + (statement ? statement->toString(indent + 1) : "{ }");
}
