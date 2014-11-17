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
				//Token::Token_type operator_type;
				void toString(std::ostream& out, unsigned int indent, std::string op, bool special = false) const;
				BinaryExpression(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
			public:
				static uptr<BinaryExpression> createBinaryExpr(uptr<Expression>& leftChild, uptr<Expression>& rightChild, Token::Token_type operator_type);
		};

		class NotEq : public BinaryExpression
		{
			private:

			public:
				NotEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Not : public BinaryExpression
		{
			private:

			public:
				Not(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LeftParen : public BinaryExpression
		{
			private:

			public:
				LeftParen(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class RightParen : public BinaryExpression
		{
			private:

			public:
				RightParen(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class MultEq : public BinaryExpression
		{
			private:

			public:
				MultEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Mult : public BinaryExpression
		{
			private:

			public:
				Mult(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class PlusPlus : public BinaryExpression
		{
			private:

			public:
				PlusPlus(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class PlusEq : public BinaryExpression
		{
			private:

			public:
				PlusEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Plus : public BinaryExpression
		{
			private:

			public:
				Plus(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Comma : public BinaryExpression
		{
			private:

			public:
				Comma(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class MinusEq : public BinaryExpression
		{
			private:

			public:
				MinusEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class MinusMinus : public BinaryExpression
		{
			private:

			public:
				MinusMinus(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Minus : public BinaryExpression
		{
			private:

			public:
				Minus(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Dot : public BinaryExpression
		{
			private:

			public:
				Dot(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class SlashEq : public BinaryExpression
		{
			private:

			public:
				SlashEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Slash : public BinaryExpression
		{
			private:

			public:
				Slash(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Colon : public BinaryExpression
		{
			private:

			public:
				Colon(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class SemiColon : public BinaryExpression
		{
			private:

			public:
				SemiColon(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThanLessThanEq : public BinaryExpression
		{
			private:

			public:
				LessThanLessThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThanLessThan : public BinaryExpression
		{
			private:

			public:
				LessThanLessThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThanEq : public BinaryExpression
		{
			private:

			public:
				LessThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LessThan : public BinaryExpression
		{
			private:

			public:
				LessThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class EqEq : public BinaryExpression
		{
			private:

			public:
				EqEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Eq : public BinaryExpression
		{
			private:

			public:
				Eq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThanEq : public BinaryExpression
		{
			private:

			public:
				GreaterThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThanGreaterThanEq : public BinaryExpression
		{
			private:

			public:
				GreaterThanGreaterThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThanGreaterThanGreaterThanEq : public BinaryExpression
		{
			private:

			public:
				GreaterThanGreaterThanGreaterThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThanGreaterThanGreaterThan : public BinaryExpression
		{
			private:

			public:
				GreaterThanGreaterThanGreaterThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThanGreaterThan : public BinaryExpression
		{
			private:

			public:
				GreaterThanGreaterThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class GreaterThan : public BinaryExpression
		{
			private:

			public:
				GreaterThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Question : public BinaryExpression
		{
			private:

			public:
				Question(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class ModEq : public BinaryExpression
		{
			private:

			public:
				ModEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Mod : public BinaryExpression
		{
			private:

			public:
				Mod(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class AndEq : public BinaryExpression
		{
			private:

			public:
				AndEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class AndAnd : public BinaryExpression
		{
			private:

			public:
				AndAnd(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class And : public BinaryExpression
		{
			private:

			public:
				And(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LeftBracket : public BinaryExpression
		{
			private:

			public:
				LeftBracket(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class RightBracket : public BinaryExpression
		{
			private:

			public:
				RightBracket(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class XorEq : public BinaryExpression
		{
			private:

			public:
				XorEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Xor : public BinaryExpression
		{
			private:

			public:
				Xor(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class LeftBrace : public BinaryExpression
		{
			private:

			public:
				LeftBrace(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class RightBrace : public BinaryExpression
		{
			private:

			public:
				RightBrace(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Neg : public BinaryExpression
		{
			private:

			public:
				Neg(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class OrEq : public BinaryExpression
		{
			private:

			public:
				OrEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class OrOr : public BinaryExpression
		{
			private:

			public:
				OrOr(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Or : public BinaryExpression
		{
			private:

			public:
				Or(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Invalid : public BinaryExpression
		{
			private:

			public:
				Invalid(uptr<Expression>& leftChild, uptr<Expression>& rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

	}
}

#endif
