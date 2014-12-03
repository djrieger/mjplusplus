#ifndef LVD_STATEMENT_HPP
#define LVD_STATEMENT_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "TypeIdent.hpp"

namespace ast
{
	class LVDStatement : public Statement
	{
		protected:
			shptr<TypeIdent> type_ident;
			shptr<Expression> init_expr;

		public:
			LVDStatement(shptr<TypeIdent> type_ident);
			LVDStatement(shptr<TypeIdent> type_ident, shptr<Expression> init_expr);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual bool analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;

			virtual unsigned int countVariableDeclarations() const;
			virtual int setVariablePositions(shptr<std::map<std::string, int>>, int) const;
			virtual void accept(ASTVisitor& visitor) const;
	};
}

#endif
