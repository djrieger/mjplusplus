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
			std::unique_ptr<Expression> leftChild;
			std::unique_ptr<Expression> rightChild;
			Token::Token_type operator_type;

		public:
			BinaryExpression(std::unique_ptr<Expression>& leftChild, std::unique_ptr<Expression>& rightChild, Token::Token_type const& operator_type);
			virtual void toString(std::ostream& out, unsigned int indent) const;
	};
}

#endif
