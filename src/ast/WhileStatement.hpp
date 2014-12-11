#ifndef WHILESTATEMENT_HPP
#define WHILESTATEMENT_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	namespace stmt
	{
		class WhileStatement : public Statement
		{
			protected:
				shptr<Expression> condition;
				shptr<Statement> statement;

			public:
				WhileStatement(shptr<Expression> condition, shptr<Statement> statement);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
				virtual bool analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symboltable) const;

				virtual unsigned int countVariableDeclarations() const;
				virtual int setVariablePositions(int) const;
				virtual void accept(ASTVisitor& visitor) const;
				virtual shptr<Expression> getCondition() const;
				virtual shptr<Statement> getLoopStatement() const;
		};
	}
}

#endif
