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
			PrimaryExpression& child;
			std::vector<PostfixOp> postfix_op;

		public:
			PostfixExpression(PrimaryExpression& child, std::vector<PostfixOp> postfix_op);
	};
}
#endif // POSTFIXEXPRESSION_HPP