#ifndef WHILESTATEMENT_HPP
#define WHILESTATEMENT_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	class WhileStatement : public Statement
	{
		protected:
			shptr<Expression> condition;
			shptr<Statement> statement;

		public:
			WhileStatement(shptr<Expression> condition, shptr<Statement> statement);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symboltable) const;
	};
}

#endif
