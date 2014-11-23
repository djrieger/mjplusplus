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
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const = 0;
				BinaryExpression(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual bool isLValue() const;
			public:
				static shptr<BinaryExpression> createBinaryExpr(shptr<Expression> leftChild, shptr<Expression> rightChild, lexer::Token::Token_type operator_type);
		};

		class NotEq : public BinaryExpression
		{
			private:

			public:
				NotEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Mult : public BinaryExpression
		{
			private:

			public:
				Mult(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Plus : public BinaryExpression
		{
			private:

			public:
				Plus(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Minus : public BinaryExpression
		{
			private:

			public:
				Minus(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Slash : public BinaryExpression
		{
			private:

			public:
				Slash(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class LessThanEq : public BinaryExpression
		{
			private:

			public:
				LessThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class LessThan : public BinaryExpression
		{
			private:

			public:
				LessThan(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class EqEq : public BinaryExpression
		{
			private:

			public:
				EqEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Eq : public BinaryExpression
		{
			private:

			public:
				Eq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class GreaterThanEq : public BinaryExpression
		{
			private:

			public:
				GreaterThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class GreaterThan : public BinaryExpression
		{
			private:

			public:
				GreaterThan(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Mod : public BinaryExpression
		{
			private:

			public:
				Mod(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class AndAnd : public BinaryExpression
		{
			private:

			public:
				AndAnd(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class OrOr : public BinaryExpression
		{
			private:

			public:
				OrOr(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Invalid : public BinaryExpression
		{
			private:

			public:
				Invalid(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

	}
}

#endif
