#include "PrimaryExpression.hpp"

namespace ast
{
	namespace pe
	{
		Bool::Bool(bool value) : value(value)
		{
			;
		}

		void Bool::toString(std::ostream& out, unsigned int, bool) const
		{
			out << (value ? "true" : "false");
		}

		Ident::Ident(std::unique_ptr<ast::Ident>& identifier) : identifier(std::move(identifier))
		{
		}

		void Ident::toString(std::ostream& out, unsigned int ident, bool) const
		{
			identifier->toString(out, ident);
		}


		Object::Object(Object_Type object_type) : object_type(object_type)
		{
		}

		void Object::toString(std::ostream& out, unsigned int, bool) const
		{
			out << ((object_type == Object_Type::THIS_OBJECT) ? "this" : "null");
		}

		Integer::Integer(std::string const& string_value)
			: string_value(string_value)
		{

		}

		void Integer::toString(std::ostream& out, unsigned int, bool) const
		{
			out << string_value;
		}


		NewArrayExpression::NewArrayExpression(std::unique_ptr<BasicType>& basic_type, std::unique_ptr<Expression>& expr, int dimension) :
			basic_type(std::move(basic_type)),
			expr(std::move(expr)),
			dimension(dimension)
		{

		}

		void NewArrayExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			basic_type->toString(out, indent);
			out << '[';
			expr->toString(out, indent, true);
			out << ']';

			if (dimension > 1)
			{
				for (int i = 0; i < dimension - 1; ++i)
					out << "[]";
			}

			out << ')';
		}

		NewObjectExpression::NewObjectExpression(std::unique_ptr<ast::Ident>& identifier) : identifier(std::move(identifier))
		{

		}

		void NewObjectExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			identifier->toString(out, indent);
			out << "())";
		}

		MethodInvocation::MethodInvocation(std::unique_ptr<ast::Ident>& identifier, std::unique_ptr<Arguments>& arguments) :
			Ident(identifier),
			arguments(std::move(arguments))
		{

		}

		void MethodInvocation::toString(std::ostream& out, unsigned int indent, bool) const
		{
			identifier->toString(out, indent);
			arguments->toString(out, indent);
		}
	} // namespace pe
} // namespace ast
