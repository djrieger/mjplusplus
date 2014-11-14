#include "IfStatement.hpp"

namespace ast
{
	IfStatement::IfStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& thenStatement)
		: condition(std::move(condition)), thenStatement(std::move(thenStatement))
	{
		;
	}
	IfStatement::IfStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& thenStatement, std::unique_ptr<Statement>& elseStatement)
		: condition(std::move(condition)), thenStatement(std::move(thenStatement)), elseStatement(std::move(elseStatement))
	{
		;
	}

	std::string IfStatement::toString(unsigned int indent) const
	{
		std::string r(indent, '\t');
		return r + "if (" + condition->toString(indent) + ") " + (thenStatement ? thenStatement->toString(indent + 1) : "{ }") + (elseStatement ? " else " + elseStatement->toString(indent + 1) : "");
	}
}
