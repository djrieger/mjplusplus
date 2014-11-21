#ifndef RETURNSTATEMENT_HPP
#define RETURNSTATEMENT_HPP

#include "../globals.hpp"
#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ReturnStatement : public Statement
	{
		private:
			shptr<Expression> expression;

		public:
			ReturnStatement();
			ReturnStatement(shptr<Expression> expression);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
	};
}

#endif
