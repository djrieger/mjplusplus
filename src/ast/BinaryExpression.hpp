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
			public:
				static shptr<BinaryExpression> createBinaryExpr(shptr<Expression> leftChild, shptr<Expression> rightChild, lexer::Token::Token_type operator_type);
				virtual bool isLValue() const;
				virtual bool standalone() const;
				virtual std::pair<bool, bool> constBool() const;
				shptr<Expression> getLeftChild() const;
				shptr<Expression> getRightChild() const;
		};

		class NotEq : public BinaryExpression
		{
			private:

			public:
				NotEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual std::pair<bool, bool> constBool() const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Mult : public BinaryExpression
		{
			private:

			public:
				Mult(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Plus : public BinaryExpression
		{
			private:

			public:
				Plus(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Minus : public BinaryExpression
		{
			private:

			public:
				Minus(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Slash : public BinaryExpression
		{
			private:

			public:
				Slash(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class LessThanEq : public BinaryExpression
		{
			private:

			public:
				LessThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class LessThan : public BinaryExpression
		{
			private:

			public:
				LessThan(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class EqEq : public BinaryExpression
		{
			private:

			public:
				EqEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual std::pair<bool, bool> constBool() const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Eq : public BinaryExpression
		{
			private:

			public:
				Eq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual bool standalone() const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class GreaterThanEq : public BinaryExpression
		{
			private:

			public:
				GreaterThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class GreaterThan : public BinaryExpression
		{
			private:

			public:
				GreaterThan(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Mod : public BinaryExpression
		{
			private:

			public:
				Mod(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class AndAnd : public BinaryExpression
		{
			private:

			public:
				AndAnd(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual std::pair<bool, bool> constBool() const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class OrOr : public BinaryExpression
		{
			private:

			public:
				OrOr(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual std::pair<bool, bool> constBool() const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Invalid : public BinaryExpression
		{
			private:

			public:
				Invalid(shptr<Expression> leftChild, shptr<Expression> rightChild);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual void accept(ASTVisitor& visitor) const;
		};

	}
}

#endif
