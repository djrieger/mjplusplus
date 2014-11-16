#include "../globals.hpp"
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

	void IfStatement::toString(std::ostream& out, unsigned int indent, bool special) const
	{
		if (!special)
			out << std::string(indent, '\t');

		bool then_block = thenStatement && thenStatement->getType() == Statement::Type::TYPE_BLOCK;

		out << "if (";
		condition->toString(out, indent, true);
		out << ')';
		out << (!thenStatement || then_block ? ' ' : '\n');

		if (thenStatement)
		{
			if (then_block)
				thenStatement->toString(out, (elseStatement ? ~indent : indent), true);
			else
				thenStatement->toString(out, indent + 1);
		}
		else
			out << "{ }\n";

		if (elseStatement)
		{
			if (!then_block)
				out << std::string(indent, '\t');

			if (elseStatement->getType() == Statement::Type::TYPE_SINGLE)
			{
				out << "else\n";
				elseStatement->toString(out, indent + 1);
			}
			else
			{
				out << "else ";
				elseStatement->toString(out, indent, true);
			}
		}

	}

	Statement::Type IfStatement::getType() const
	{
		return Type::TYPE_IF;
	}
}
