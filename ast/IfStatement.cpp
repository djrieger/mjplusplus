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
		bool then_block = thenStatement && thenStatement->getType() == Statement::Type::TYPE_BLOCK;

		r += "if (" + condition->toString(indent) + ')';
		r += then_block ? ' ' : '\n';
		r += (thenStatement ? thenStatement->toString(indent + 1) : "{ }");

		if (elseStatement && then_block)
			r[r.size() - 1] = ' ';// replace newline after block, else goes on the same line

		//TODO: remove indentation from else if
		r += (elseStatement ? (then_block ? "" : std::string(indent, '\t')) + "else" +
		      (elseStatement->getType() == Statement::Type::TYPE_SINGLE ? '\n' : ' ') +
		      elseStatement->toString(indent + (elseStatement->getType() == Statement::Type::TYPE_IF ? 0 : 1))
		      : ""
		     );
		return r;
	}

	Statement::Type IfStatement::getType() const
	{
		return Type::TYPE_IF;
	}
}
