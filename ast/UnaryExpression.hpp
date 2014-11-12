#include "Node.hpp"
#include <vector>

namespace ast
{
	class UnaryExpression : public Expression
	{
		private:
			PostfixExpression& child;
			std::vector<Unary_Operator> unary_operators;


		public:
			UnaryExpression(PostfixExpression& child, std::vector<Unary_Operator> unary_operators);

			enum Unary_Operator
			{
				UNARY_NOT,
				UNARY_MINUS
			};
	}
}