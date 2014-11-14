#include "PrimaryExpression.hpp"

namespace ast
{
	namespace pe
	{
		Bool::Bool(bool value) : value(value)
		{
			;
		}

		std::string Bool::toString() const
		{
			return value ? "true" : "false";
		}

		Ident::Ident(std::unique_ptr<ast::Ident>& identifier) : identifier(std::move(identifier))
		{
		}

		std::string Ident::toString() const
		{
			return identifier->toString();
		}


		Object::Object(Object_Type object_type) : object_type(object_type)
		{
		}

		std::string Object::toString() const
		{
			return (object_type == Object_Type::THIS_OBJECT) ? "this" : "null";
		}

		Integer::Integer(std::string const& string_value)
			: string_value(string_value)
		{

		}

		std::string Integer::toString() const
		{
			return string_value;
		}


		NewArrayExpression::NewArrayExpression(std::unique_ptr<BasicType>& basic_type, std::unique_ptr<Expression>& expr, int dimension) :
			basic_type(std::move(basic_type)),
			expr(std::move(expr)),
			dimension(dimension)
		{

		}

		std::string NewArrayExpression::toString() const
		{
			std::string s = "";

			if (dimension > 1)
			{
				for (int i = 0; i < dimension - 1; ++i)
					s += "[]";
			}

			return s;
		}

		NewObjectExpression::NewObjectExpression(std::unique_ptr<ast::Ident>& identifier) : identifier(std::move(identifier))
		{

		}

		std::string NewObjectExpression::toString() const
		{
			return "new " + identifier->toString() + "()";
		}

		MethodInvocation::MethodInvocation(std::unique_ptr<ast::Ident>& identifier, std::unique_ptr<Arguments>& arguments) :
			Ident(identifier),
			arguments(std::move(arguments))
		{

		}

		std::string MethodInvocation::toString() const
		{
			return identifier->toString() + arguments->toString();
		}

		ExpressionWithParens::ExpressionWithParens(std::unique_ptr<Expression>& expr) : expr(std::move(expr))
		{

		}

		std::string ExpressionWithParens::toString() const
		{
			return "(" + expr->toString() + ")";
		}

	} // namespace pe
} // namespace ast
