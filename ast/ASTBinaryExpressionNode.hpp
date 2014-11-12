#include "Node.hpp"
#include <vector>

namespace ast
{
	class BinaryExpression : public Expression
	{
		private:
			Expression& leftChild;
			Expression& rightChild;
			Token::Token_type operator_type;

		public:
			BinaryExpression(Expression& leftChild, Expression& rightChild, Token::Token_type operator_type);

	}
}