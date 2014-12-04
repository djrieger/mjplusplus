#ifndef PRIMARYEXPRESSION_HPP
#define PRIMARYEXPRESSION_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Arguments.hpp"
#include "Ident.hpp"
#include "IdentBase.hpp"
#include "Type.hpp"
#include "MethodInvocationBase.hpp"
#include <vector>

namespace ast
{

	namespace pe
	{
		class PrimaryExpression : public Expression
		{
			public:
				PrimaryExpression(source_position_t position);
				virtual bool standalone() const;
				virtual std::pair<bool, bool> constBool() const;
		};

		class Bool : public PrimaryExpression
		{
			private:
				bool value;
			public:
				Bool(bool value, source_position_t position);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual bool isLValue() const;
				virtual std::pair<bool, bool> constBool() const;
				virtual bool getValue() const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class Ident : public PrimaryExpression, public IdentBase
		{
			public:
				Ident(shptr<ast::Ident> identifier);
				virtual void toString(std::ostream& out, unsigned int ident, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual bool isLValue() const;
				virtual void accept(ASTVisitor& visitor) const;
				virtual std::string const& getIdentifier() const;
			private:
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

				Object(Object_Type object_type, source_position_t position);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual bool isLValue() const;
				virtual void accept(ASTVisitor& visitor) const;
				virtual Object_Type getObjectType() const;
			private:
				Object_Type object_type;
		};

		class Integer : public PrimaryExpression
		{
			private:
				std::string const& string_value;
			public:
				Integer(std::string const& string_value, source_position_t position);
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual bool isLValue() const;
				virtual std::string const& getStringValue() const;
				virtual void accept(ASTVisitor& visitor) const;
		};

		class NewArrayExpression : public PrimaryExpression
		{
			public:
				NewArrayExpression(shptr<Type> type, shptr<Expression> expr);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual bool isLValue() const;
				virtual void accept(ASTVisitor& visitor) const;
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
				virtual bool isLValue() const;
				virtual bool standalone() const;
				virtual void accept(ASTVisitor& visitor) const;
			private:
				shptr<ast::Ident> identifier;
		};

		class MethodInvocation : public PrimaryExpression, public MethodInvocationBase
		{
			public:
				MethodInvocation(shptr<ast::Ident> method_name, shptr<Arguments> arguments);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
				virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
				virtual bool isLValue() const;
				virtual bool standalone() const;
				virtual void accept(ASTVisitor& visitor) const;
		};
	} // namespace pe

} // namespace ast

#endif // PRIMARYEXPRESSION_HPP
