#ifndef PRIMARYEXPRESSION_HPP
#define PRIMARYEXPRESSION_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Arguments.hpp"
#include "Ident.hpp"
#include "Type.hpp"
#include <vector>

namespace ast
{

	namespace pe
	{
		class PrimaryExpression : public Expression
		{

		};

		class Bool : public PrimaryExpression
		{
			private:
				bool value;
			public:
				Bool(bool value);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class Ident : public PrimaryExpression
		{
			public:
				Ident(shptr<ast::Ident> identifier);
				virtual void toString(std::ostream& out, unsigned int ident, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			protected:
				shptr<ast::Ident> identifier;
		};

		class Object : public PrimaryExpression
		{
			public:
				enum Object_Type
				{
					NULL_OBJECT,
					THIS_OBJECT
				};

				Object(Object_Type object_type);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			private:
				Object_Type object_type;
		};

		class Integer : public PrimaryExpression
		{
			private:
				std::string const& string_value;
			public:
				Integer(std::string const& string_value);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
		};

		class NewArrayExpression : public PrimaryExpression
		{
			public:
				NewArrayExpression(shptr<Type> type, shptr<Expression> expr);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			private:
				shptr<Type> type;
				shptr<Expression> expr;
		};

		class NewObjectExpression : public PrimaryExpression
		{
			public:
				NewObjectExpression(shptr<ast::Ident> identifier);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			private:
				shptr<ast::Ident> identifier;
		};

		class MethodInvocation : public Ident
		{
			public:
				MethodInvocation(shptr<ast::Ident> identifier, shptr<Arguments> arguments);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			private:
				shptr<Arguments> arguments;
		};
	} // namespace pe

} // namespace ast

#endif // PRIMARYEXPRESSION_HPP
