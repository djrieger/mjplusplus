#include "../globals.hpp"
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

		Ident::Ident(uptr<ast::Ident> identifier) : identifier(std::move(identifier))
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


		NewArrayExpression::NewArrayExpression(uptr<Type> type, uptr<Expression> expr) :
			type(std::move(type)),
			expr(std::move(expr))
		{

		}

		void NewArrayExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			type->toString(out, indent, &expr);
			out << ')';
		}

		NewObjectExpression::NewObjectExpression(uptr<ast::Ident> identifier) : identifier(std::move(identifier))
		{

		}

		void NewObjectExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			identifier->toString(out, indent);
			out << "())";
		}

		MethodInvocation::MethodInvocation(uptr<ast::Ident> identifier, uptr<Arguments> arguments) :
			Ident(std::move(identifier)),
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
