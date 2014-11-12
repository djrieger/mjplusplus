#ifndef PRIMARYEXPRESSION_HPP
#define PRIMARYEXPRESSION_HPP

#include "Expression.hpp"
#include <vector>

namespace ast
{
	class PrimaryExpression : public Expression
	{
		private:
			Node& child;
			std::vector<Expression> arguments;

		public:
			PrimaryExpression(Node& child, std::vector<Expression> arguments);
			std::string toString();
	};
}

#endif // PRIMARYEXPRESSION_HPP