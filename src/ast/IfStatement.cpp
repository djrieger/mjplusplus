#include "../globals.hpp"
#include "IfStatement.hpp"

namespace ast
{
	IfStatement::IfStatement(shptr<Expression> condition, shptr<Statement> thenStatement)
		: condition(condition), thenStatement(thenStatement)
	{
		;
	}
	IfStatement::IfStatement(shptr<Expression> condition, shptr<Statement> thenStatement, shptr<Statement> elseStatement)
		: condition(condition), thenStatement(thenStatement), elseStatement(elseStatement)
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

	bool IfStatement::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
	{
		// TODO: if we can determine the condition at compile time, one statement doesn't matter for definite return analysis
		// still perform sementic analysis though
		auto cond = condition->get_type(sa, symbolTable);

		if (!cond || *cond != ast::Type(ast::Type::Primitive_type::BOOLEAN))
			sa.printError("If condition is not boolean");

		bool returns = true;
		if (thenStatement)
			returns = thenStatement->analyze(sa, symbolTable) && returns;
		else
			returns = false;

		if (elseStatement)
			returns = elseStatement->analyze(sa, symbolTable) && returns;
		else
			returns = false;

		return returns;
	}
}
