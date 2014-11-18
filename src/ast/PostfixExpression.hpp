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
			uptr<Expression> child;
			uptr<vec<uptr<PostfixOp>>> postfix_op;

		public:
			PostfixExpression(uptr<Expression>& child, uptr<vec<uptr<PostfixOp>>>& postfix_op);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
	};
}
#endif // POSTFIXEXPRESSION_HPP
