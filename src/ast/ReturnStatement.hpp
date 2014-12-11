#ifndef RETURNSTATEMENT_HPP
#define RETURNSTATEMENT_HPP

#include "../globals.hpp"
#include "Statement.hpp"
#include "Expression.hpp"
#include "PositionAwareNode.hpp"

namespace ast
{
	namespace stmt
	{
		class ReturnStatement : public Statement, public PositionAwareNode
		{
			private:
				shptr<Expression> expression;

			public:
				ReturnStatement(source_position_t position);
				ReturnStatement(shptr<Expression> expression);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
				virtual bool analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
				shptr<Expression> getExpression() const;
		};
	}
}

#endif
