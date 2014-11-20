#ifndef BINARY_EXPRESSION_HPP
#define BINARY_EXPRESSION_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Expression.hpp"
#include "../lexer/token.hpp"
#include <vector>

namespace ast
{
	namespace be
	{
		class BinaryExpression : public Expression
		{
			protected:
				shptr<Expression> leftChild;
				shptr<Expression> rightChild;
				void toString(std::ostream& out, unsigned int indent, std::string op, bool special = false) const;
				BinaryExpression(shptr<Expression> leftChild, shptr<Expression> rightChild);
			public:
				static shptr<BinaryExpression> createBinaryExpr(shptr<Expression> leftChild, shptr<Expression> rightChild, lexer::Token::Token_type operator_type);
		};

		class NotEq : public BinaryExpression
		{
			private:

			public:
				NotEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Mult : public BinaryExpression
		{
			private:

			public:
				Mult(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Plus : public BinaryExpression
		{
			private:

			public:
				Plus(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Minus : public BinaryExpression
		{
			private:

			public:
				Minus(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Slash : public BinaryExpression
		{
			private:

			public:
				Slash(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThanEq : public BinaryExpression
		{
			private:

			public:
				LessThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThan : public BinaryExpression
		{
			private:

			public:
				LessThan(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class EqEq : public BinaryExpression
		{
			private:

			public:
				EqEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Eq : public BinaryExpression
		{
			private:

			public:
				Eq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThanEq : public BinaryExpression
		{
			private:

			public:
				GreaterThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThan : public BinaryExpression
		{
			private:

			public:
				GreaterThan(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Mod : public BinaryExpression
		{
			private:

			public:
				Mod(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class AndAnd : public BinaryExpression
		{
			private:

			public:
				AndAnd(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class OrOr : public BinaryExpression
		{
			private:

			public:
				OrOr(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Invalid : public BinaryExpression
		{
			private:

			public:
				Invalid(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

	}
}

#endif
