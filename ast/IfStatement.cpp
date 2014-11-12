#include "IfStatement.hpp"

namespace ast
{
	IfStatement::IfStatement(Expression& condition, Statement& thenStatement) : condition(condition), thenStatement(thenStatement)
	{
		;
	}

	std::string IfStatement::toString()
	{
		return "if (" + condition.toString() + ")" + thenStatement.toString();
	}
}
