#ifndef IFSTATEMENT_HPP
#define IFSTATEMENT_HPP

#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
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
			virtual bool analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			virtual unsigned int countVariableDeclarations() const;
			virtual void accept(ASTVisitor& visitor) const;
	};
}

#endif
