#include "WhileStatement.hpp"

ast::WhileStatement::WhileStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& statement) :
	condition(std::move(condition)), statement(std::move(statement))
{

}

void ast::WhileStatement::toString(std::ostream& out, unsigned int indent, bool special) const
{
	if (!special)
		out << std::string(indent, '\t');

	bool block = statement && statement->getType() == Statement::Type::TYPE_BLOCK;

	out << "while (";
	condition->toString(out, indent, true);
	out << ')';
	out << (!statement || block ? ' ' : '\n');

	if (statement)
	{
		if (block)
			statement->toString(out, indent, true);
		else
			statement->toString(out, indent + 1);
	}
	else
		out << "{ }\n";
}
