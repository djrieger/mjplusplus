#ifndef POSTFIXEXPRESSION_HPP
#define POSTFIXEXPRESSION_HPP
#include <vector>
#include "Expression.hpp"
#include "PrimaryExpression.hpp"
#include "PostfixOp.hpp"

namespace ast
{
	class PostfixExpression : public Expression
	{
		private:
			shptr<Expression> child;
			shptr<vec<shptr<PostfixOp>>> postfix_op;

		public:
			PostfixExpression(shptr<Expression> child, shptr<vec<shptr<PostfixOp> > > postfix_op);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
	};
}
#endif // POSTFIXEXPRESSION_HPP
