#ifndef LVD_STATEMENT_HPP
#define LVD_STATEMENT_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "TypeIdent.hpp"
#include "VariableDeclaration.hpp"

namespace ast
{
	class LVDStatement : public Statement, public VariableDeclaration
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
			virtual int setVariablePositions(int) const;
			virtual void accept(ASTVisitor& visitor) const;
			virtual shptr<ast::Type> const& getDeclType() const;
	};
}

#endif
