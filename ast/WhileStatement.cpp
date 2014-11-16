#include "WhileStatement.hpp"

ast::WhileStatement::WhileStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& statement) :
	condition(std::move(condition)), statement(std::move(statement))
{

}

void ast::WhileStatement::toString(std::ostream& out, unsigned int indent, bool special) const
{
	out << std::string(indent, '\t') << "while (";
	condition->toString(out, indent);
	out << ") ";

	if (statement)
		statement->toString(out, indent + 1);
	else
		out << "{ }";
}
