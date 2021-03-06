#include "../globals.hpp"
#include "IfStatement.hpp"

namespace ast
{
	namespace stmt
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

		bool IfStatement::analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const
		{
			// TODO: if we can determine the condition at compile time, one statement doesn't matter for definite return analysis
			// still perform semantic analysis though
			auto cond = condition->get_type(sa, symbolTable);

			if (!cond || *cond != ast::Type(ast::Type::Primitive_type::BOOLEAN)) //TODO: isBool()
				sa.reportError("If condition is not boolean", condition);

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

		unsigned int IfStatement::countVariableDeclarations() const
		{
			return (thenStatement ? thenStatement->countVariableDeclarations() : 0)
			       + (elseStatement ? elseStatement->countVariableDeclarations() : 0);
		}

		int IfStatement::setVariablePositions(int pos) const
		{

			if (thenStatement)
				pos = thenStatement->setVariablePositions(pos);

			if (elseStatement)
				pos = elseStatement->setVariablePositions(pos);

			return pos;
		}

		void IfStatement::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<IfStatement const>(shared_from_this()));
		}

		shptr<Expression> IfStatement::getCondition() const
		{
			return this->condition;
		}

		shptr<Statement> IfStatement::getThenStatement() const
		{
			return this->thenStatement;
		}

		shptr<Statement> IfStatement::getElseStatement() const
		{
			return this->elseStatement;
		}
	}
}
