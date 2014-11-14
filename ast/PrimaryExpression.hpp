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
				virtual std::string toString() const;
		};

		class Ident : public PrimaryExpression
		{
			public:
				Ident(std::unique_ptr<Ident>& identifier);
				virtual std::string toString() const;
			protected:
				std::unique_ptr<Ident> identifier;
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
				virtual std::string toString() const;
			private:
				Object_Type object_type;
		};

		class Integer : public PrimaryExpression
		{
			private:
				std::string string_value;
			public:
				Integer(std::string const& string_value);
				virtual std::string toString() const;
		};

		class NewArrayExpression : public PrimaryExpression
		{
			public:
				NewArrayExpression(std::unique_ptr<BasicType>& basic_type, std::unique_ptr<Expression>& expr, int dimension);
				virtual std::string toString() const;
			private:
				std::unique_ptr<BasicType> basic_type;
				std::unique_ptr<Expression> expr;
				int dimension;
		};

		class NewObjectExpression : public PrimaryExpression
		{
			public:
				NewObjectExpression(std::unique_ptr<Ident>& identifier);
				virtual std::string toString() const;
			private:
				std::unique_ptr<Ident> identifier;
		};

		class IdentWithArguments : public Ident
		{
			public:
				IdentWithArguments(std::unique_ptr<Ident>& identifier, std::unique_ptr<Arguments>& arguments);
				virtual std::string toString() const;
			private:
				std::unique_ptr<Ident> identifier;
				std::unique_ptr<Arguments> arguments;
		};

		class ExpressionWithParens : public PrimaryExpression
		{
			public:
				ExpressionWithParens(std::unique_ptr<Expression>& expr);
				virtual std::string toString() const;
			private:
				std::unique_ptr<Expression> expr;
		};
	} // namespace pe

} // namespace ast

#endif // PRIMARYEXPRESSION_HPP