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
			std::unique_ptr<Expression> child;
			std::unique_ptr<std::vector<std::unique_ptr<PostfixOp>>> postfix_op;

		public:
			PostfixExpression(std::unique_ptr<Expression>& child, std::unique_ptr<std::vector<std::unique_ptr<PostfixOp>>>& postfix_op);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
	};
}
#endif // POSTFIXEXPRESSION_HPP
