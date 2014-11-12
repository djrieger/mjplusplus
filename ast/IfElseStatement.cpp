#include "IfElseStatement.hpp"

namespace ast
{
	IfElseStatement::IfElseStatement(Expression& condition, Statement& thenStatement, Statement& elseStatement) : IfStatement(condition, thenStatement), elseStatement(elseStatement)
	{
		;
	}

	std::string IfElseStatement::toString()
	{
		return "if (" + condition.toString() + ")" + thenStatement.toString() + "else " + elseStatement.toString();
	}
}
