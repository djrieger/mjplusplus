#ifndef UNARYEXPRESSION_HPP
#define UNARYEXPRESSION_HPP

#include "../globals.hpp"
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
			UnaryExpression(uptr<Expression> child, uptr<vec<Unary_Operator>> unary_operators);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;

		private:
			uptr<Expression> child;
			uptr<vec<Unary_Operator>> unary_operators;

	};
}

#endif
