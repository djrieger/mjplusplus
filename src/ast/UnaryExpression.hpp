#ifndef UNARYEXPRESSION_HPP
#define UNARYEXPRESSION_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Expression.hpp"
#include "PostfixExpression.hpp"
#include "../lexer/token.hpp"
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
				UnaryExpression(shptr<Expression> child, int size);
				void toString(std::ostream& out, unsigned int indent, std::string op, bool special = false) const;
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const = 0;

				shptr<Expression> child;
				int size;
			public:

				/**
				 * Creates an Expression using the given child expression and the operator vector.
				 * If the operator vector is empty, the child is returned. Else the corresponding UnaryExpression is returned.
				 * @param child child expression
				 * @param operator_types operator vector
				 * @return expr, either the child or an UnaryExpression
				 */
				static shptr<Expression> createUnaryExpr(shptr<Expression> child, shptr<vec<lexer::Token::Token_type>> operator_types);
				virtual bool isLValue() const;
				virtual bool standalone() const;

		};

		class Not : public UnaryExpression
		{
			public:
				Not(shptr<Expression> child, int size);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Neg : public UnaryExpression
		{
			public:
				Neg(shptr<Expression> child, int size);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

	}
}

#endif
