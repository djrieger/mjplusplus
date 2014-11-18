#ifndef PRIMARYEXPRESSION_HPP
#define PRIMARYEXPRESSION_HPP

#include "../globals.hpp"
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
				virtual void toString(std::ostream& out, unsigned int, bool = false) const;
		};

		class Ident : public PrimaryExpression
		{
			public:
				Ident(uptr<ast::Ident> identifier);
				virtual void toString(std::ostream& out, unsigned int ident, bool = false) const;
			protected:
				uptr<ast::Ident> identifier;
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
		};

		class NewArrayExpression : public PrimaryExpression
		{
			public:
				NewArrayExpression(uptr<BasicType> basic_type, uptr<Expression> expr, int dimension);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			private:
				uptr<BasicType> basic_type;
				uptr<Expression> expr;
				int dimension;
		};

		class NewObjectExpression : public PrimaryExpression
		{
			public:
				NewObjectExpression(uptr<ast::Ident> identifier);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			private:
				uptr<ast::Ident> identifier;
		};

		class MethodInvocation : public Ident
		{
			public:
				MethodInvocation(uptr<ast::Ident> identifier, uptr<Arguments> arguments);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			private:
				uptr<Arguments> arguments;
		};
	} // namespace pe

} // namespace ast

#endif // PRIMARYEXPRESSION_HPP
