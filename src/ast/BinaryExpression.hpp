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
				uptr<Expression> leftChild;
				uptr<Expression> rightChild;
				void toString(std::ostream& out, unsigned int indent, std::string op, bool special = false) const;
				BinaryExpression(uptr<Expression> leftChild, uptr<Expression> rightChild);
			public:
				static uptr<BinaryExpression> createBinaryExpr(uptr<Expression> leftChild, uptr<Expression> rightChild, Token::Token_type operator_type);
		};

		class NotEq : public BinaryExpression
		{
			private:

			public:
				NotEq(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Mult : public BinaryExpression
		{
			private:

			public:
				Mult(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Plus : public BinaryExpression
		{
			private:

			public:
				Plus(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Minus : public BinaryExpression
		{
			private:

			public:
				Minus(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Slash : public BinaryExpression
		{
			private:

			public:
				Slash(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThanEq : public BinaryExpression
		{
			private:

			public:
				LessThanEq(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThan : public BinaryExpression
		{
			private:

			public:
				LessThan(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class EqEq : public BinaryExpression
		{
			private:

			public:
				EqEq(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Eq : public BinaryExpression
		{
			private:

			public:
				Eq(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThanEq : public BinaryExpression
		{
			private:

			public:
				GreaterThanEq(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThan : public BinaryExpression
		{
			private:

			public:
				GreaterThan(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Mod : public BinaryExpression
		{
			private:

			public:
				Mod(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class AndAnd : public BinaryExpression
		{
			private:

			public:
				AndAnd(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class OrOr : public BinaryExpression
		{
			private:

			public:
				OrOr(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Invalid : public BinaryExpression
		{
			private:

			public:
				Invalid(uptr<Expression> leftChild, uptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

	}
}

#endif
