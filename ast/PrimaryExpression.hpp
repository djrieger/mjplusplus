#ifndef PRIMARYEXPRESSION_HPP
#define PRIMARYEXPRESSION_HPP

#include "Expression.hpp"
#include "Arguments.hpp"
#include "Ident.hpp"
#include "BasicType.hpp"
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
				virtual void toString(std::ostream& out, unsigned int, bool special = false) const;
		};

		class Ident : public PrimaryExpression
		{
			public:
				Ident(std::unique_ptr<ast::Ident>& identifier);
				virtual void toString(std::ostream& out, unsigned int ident, bool special = false) const;
			protected:
				std::unique_ptr<ast::Ident> identifier;
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
				virtual void toString(std::ostream& out, unsigned int, bool special = false) const;
			private:
				Object_Type object_type;
		};

		class Integer : public PrimaryExpression
		{
			private:
				std::string string_value;
			public:
				Integer(std::string const& string_value);
				virtual void toString(std::ostream& out, unsigned int, bool special = false) const;
		};

		class NewArrayExpression : public PrimaryExpression
		{
			public:
				NewArrayExpression(std::unique_ptr<BasicType>& basic_type, std::unique_ptr<Expression>& expr, int dimension);
				virtual void toString(std::ostream& out, unsigned int, bool special = false) const;
			private:
				std::unique_ptr<BasicType> basic_type;
				std::unique_ptr<Expression> expr;
				int dimension;
		};

		class NewObjectExpression : public PrimaryExpression
		{
			public:
				NewObjectExpression(std::unique_ptr<ast::Ident>& identifier);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			private:
				std::unique_ptr<ast::Ident> identifier;
		};

		class MethodInvocation : public Ident
		{
			public:
				MethodInvocation(std::unique_ptr<ast::Ident>& identifier, std::unique_ptr<Arguments>& arguments);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			private:
				std::unique_ptr<Arguments> arguments;
		};

		class ExpressionWithParens : public PrimaryExpression
		{
			public:
				ExpressionWithParens(std::unique_ptr<Expression>& expr);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			private:
				std::unique_ptr<Expression> expr;
		};
	} // namespace pe

} // namespace ast

#endif // PRIMARYEXPRESSION_HPP
