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
			std::unique_ptr<PrimaryExpression> child;
			std::unique_ptr<std::vector<PostfixOp>> postfix_op;

		public:
			PostfixExpression(std::unique_ptr<PrimaryExpression>& child, std::unique_ptr<std::vector<PostfixOp>>& postfix_op);
			virtual std::string toString();
	};
}
#endif // POSTFIXEXPRESSION_HPP