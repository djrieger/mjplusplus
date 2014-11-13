#ifndef PRIMARYEXPRESSION_HPP
#define PRIMARYEXPRESSION_HPP

#include "Expression.hpp"
#include <vector>

namespace ast
{
	class PrimaryExpression : public Expression
	{
		private:
			std::unique_ptr<Node> child;
			std::unique_ptr<std::vector<Expression>> arguments;

		public:
			PrimaryExpression(std::unique_ptr<Node>& child, std::unique_ptr<std::vector<Expression>>& arguments);
			virtual std::string toString();
	};
}

#endif // PRIMARYEXPRESSION_HPP