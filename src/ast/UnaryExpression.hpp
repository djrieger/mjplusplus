#ifndef UNARYEXPRESSION_HPP
#define UNARYEXPRESSION_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Expression.hpp"
#include "PostfixExpression.hpp"
#include <vector>

namespace ast
{
	namespace ue
	{
		class UnaryExpression : public Expression
		{
			protected:
				enum Unary_Operator
				{
					UNARY_NOT,
					UNARY_MINUS
				};

				//We could replace the unary-operator vector to a token_type vector to move the conversion into this class.
				UnaryExpression(uptr<Expression> child, int size);
				virtual void toString(std::ostream& out, unsigned int indent, std::string op, bool special = false) const;

				uptr<Expression> child;
				int size;
			public:

				/**
				 * Creates an Expression using the given child expression and the operator vector.
				 * If the operator vector is empty, the child is returned. Else the corresponding UnaryExpression is returned.
				 * @param child child expression
				 * @param operator_types operator vector
				 * @return expr, either the child or an UnaryExpression
				 */
				static uptr<Expression> createUnaryExpr(uptr<Expression> child, uptr<vec<Token::Token_type>> operator_types);

		};

		class Not : public UnaryExpression
		{
			public:
				Not(uptr<Expression> child, int size);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
		};

		class Neg : public UnaryExpression
		{
			public:
				Neg(uptr<Expression> child, int size);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
		};

	}
}

#endif
