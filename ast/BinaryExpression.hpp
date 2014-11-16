#ifndef BINARY_EXPRESSION_HPP
#define BINARY_EXPRESSION_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Expression.hpp"
#include "../lexer/token.hpp"
#include <vector>

namespace ast
{
	class BinaryExpression : public Expression
	{
		private:
			uptr<Expression> leftChild;
			uptr<Expression> rightChild;
			Token::Token_type operator_type;

		public:
			BinaryExpression(uptr<Expression>& leftChild, uptr<Expression>& rightChild, Token::Token_type const& operator_type);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
	};
}

#endif
