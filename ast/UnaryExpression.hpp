#ifndef UNARYEXPRESSION_HPP
#define UNARYEXPRESSION_HPP

#include "Node.hpp"
#include "Expression.hpp"
#include "PostfixExpression.hpp"
#include <vector>

namespace ast
{
	class UnaryExpression : public Expression
	{
		public:
			enum Unary_Operator
			{
				UNARY_NOT,
				UNARY_MINUS
			};

			UnaryExpression(PostfixExpression& child, std::vector<Unary_Operator> unary_operators);
			virtual std::string toString();

		private:
			PostfixExpression& child;
			std::vector<Unary_Operator> unary_operators;

	};
}

#endif