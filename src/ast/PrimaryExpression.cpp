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

		bool Bool::check_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			return true;
		}

		Type Bool::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			return Type(Type::BOOLEAN);
		}

		void Bool::toString(std::ostream& out, unsigned int, bool) const
		{
			out << (value ? "true" : "false");
		}

		Ident::Ident(shptr<ast::Ident> identifier) : identifier(identifier)
		{
		}

		void Ident::toString(std::ostream& out, unsigned int ident, bool) const
		{
			identifier->toString(out, ident);
		}

		bool Ident::check_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			throw 42;
		}

		Type Ident::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			throw 42;
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

		bool Integer::check_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			return true;
		}

		Type Integer::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			return Type(Type::INT);
		}

		void Integer::toString(std::ostream& out, unsigned int, bool) const
		{
			out << string_value;
		}

		NewArrayExpression::NewArrayExpression(shptr<Type> type, shptr<Expression> expr) :
			type(type),
			expr(expr)
		{

		}

		bool NewArrayExpression::check_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			if (child->check_type(sa, symbolTable) && (child->get_type(sa, symbolTable) == Type(Type::INT)))
			{
				//chekc type
				auto symbol = Symbol::makeSymbol(this->`)
			}

			return false;
		}

		Type NewArrayExpression::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			return Type(Type::INT);
		}


		void NewArrayExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			type->toString(out, indent, &expr);
			out << ')';
		}

		NewObjectExpression::NewObjectExpression(shptr<ast::Ident> identifier) : identifier(identifier)
		{

		}

		void NewObjectExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			identifier->toString(out, indent);
			out << "())";
		}

		MethodInvocation::MethodInvocation(shptr<ast::Ident> identifier, shptr<Arguments> arguments) :
			Ident(identifier),
			arguments(arguments)
		{

		}

		void MethodInvocation::toString(std::ostream& out, unsigned int indent, bool) const
		{
			identifier->toString(out, indent);
			arguments->toString(out, indent);
		}
	} // namespace pe
} // namespace ast
