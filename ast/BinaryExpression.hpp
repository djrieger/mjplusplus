#ifndef BINARY_EXPRESSION_HPP
#define BINARY_EXPRESSION_HPP

#include "Node.hpp"
#include "Expression.hpp"
#include "../lexer/token.hpp"
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

	};
}

#endif
