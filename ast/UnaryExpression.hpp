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

			//We could replace the unary-operator vector to a token_type vector to move the conversion into this class.
			UnaryExpression(std::unique_ptr<Expression>& child, std::unique_ptr<std::vector<Unary_Operator>>& unary_operators);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;

		private:
			std::unique_ptr<Expression> child;
			std::unique_ptr<std::vector<Unary_Operator>> unary_operators;

	};
}

#endif
