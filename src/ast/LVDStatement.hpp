#ifndef LVD_STATEMENT_HPP
#define LVD_STATEMENT_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "TypeIdent.hpp"
#include "VariableDeclaration.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	namespace stmt
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
				virtual bool analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;

				shptr<Ident> getIdent() const;
				virtual unsigned int countVariableDeclarations() const;
				virtual int setVariablePositions(int) const;
				virtual void accept(ASTVisitor& visitor) const;
				virtual shptr<ast::Type> const& getDeclType() const;
				shptr<ast::Expression> const& getInitialization() const;
		};
	}
}

#endif
