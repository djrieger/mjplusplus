#ifndef IFSTATEMENT_HPP
#define IFSTATEMENT_HPP

#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	namespace stmt
	{
		class IfStatement : public Statement
		{
			private:
				shptr<Expression> condition;
				shptr<Statement> thenStatement;
				shptr<Statement> elseStatement;

			public:
				IfStatement(shptr<Expression> condition, shptr<Statement> thenStatement);
				IfStatement(shptr<Expression> condition, shptr<Statement> thenStatement, shptr<Statement> elseStatement);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
				virtual Statement::Type getType() const;
				virtual bool analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
				virtual unsigned int countVariableDeclarations() const;
				virtual int setVariablePositions(int) const;
				virtual void accept(ASTVisitor& visitor) const;

				virtual shptr<Expression> getCondition() const;
				virtual shptr<Statement> getThenStatement() const;
				virtual shptr<Statement> getElseStatement() const;
		};
	}
}

#endif
