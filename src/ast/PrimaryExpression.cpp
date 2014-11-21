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

		shptr<Type> Bool::get_type(SemanticAnalysis&, shptr<SymbolTable>) const
		{
			//TODO: is there a better way than to create a new shared pointer for every boolean or int?
			return std::make_shared<Type>(Type::BOOLEAN);
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

		shptr<Type> Ident::get_type(SemanticAnalysis& sa, shptr<SymbolTable>) const
		{
			shptr<Symbol> ident_symbol = Symbol::makeSymbol(identifier->getName());

			if (ident_symbol)
			{
				shptr<Definition> ident_def = ident_symbol->getCurrentDefinition();

				if (ident_def)
				{
					shptr<Type> ident_type = ident_def->getType();

					if (sa.isTypeDefined(ident_type))
						return ident_type;
				}
				else
					sa.printError("No current definition for " + identifier->getName());
			}

			return shptr<Type>();
		}


		Object::Object(Object_Type object_type) : object_type(object_type)
		{
		}

		void Object::toString(std::ostream& out, unsigned int, bool) const
		{
			out << ((object_type == Object_Type::THIS_OBJECT) ? "this" : "null");
		}

		shptr<Type> Object::get_type(SemanticAnalysis& sa, shptr<SymbolTable>) const
		{
			if (object_type == Object_Type::THIS_OBJECT)
			{
				shptr<Symbol> this_symbol = Symbol::makeSymbol("this");

				if (this_symbol)
				{
					shptr<Definition> this_def = this_symbol->getCurrentDefinition();

					if (this_def)
					{
						shptr<Type> this_type = this_def->getType();

						if (sa.isTypeDefined(this_type))
							return this_type;
					}
				}
			}
			else if (object_type == Object_Type::NULL_OBJECT)
			{
				//TODO: define a special type-value that has some special rules for the java "null"-object
			}

			return shptr<Type>();
		}

		Integer::Integer(std::string const& string_value)
			: string_value(string_value)
		{

		}

		shptr<Type>Integer::get_type(SemanticAnalysis&, shptr<SymbolTable>) const
		{
			return std::make_shared<Type>(Type::INT);
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

		shptr<Type>NewArrayExpression::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			shptr<Type> child_type = expr->get_type(sa, symbolTable);

			if (!child_type)
				return child_type;
			else if (*child_type == Type(Type::INT) && sa.isTypeDefined(type))
				return type;
			else
				return shptr<Type>();
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

		shptr<Type> NewObjectExpression::get_type(SemanticAnalysis& sa, shptr<SymbolTable>) const
		{
			shptr<Type> type = std::make_shared<Type>(identifier);

			if (sa.isTypeDefined(type))
				return type;
			else
				return shptr<Type>();
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

		shptr<Type> MethodInvocation::get_type(SemanticAnalysis& sa, shptr<SymbolTable>) const
		{
			shptr<Symbol> methodSymbol = Symbol::makeSymbol(identifier->getName());

			if (methodSymbol)
			{
				shptr<Definition> method_def = methodSymbol->getCurrentDefinition();

				if (method_def)
				{
					shptr<Type> method_type = method_def->getType();

					if (sa.isTypeDefined(method_type))
					{
						//Now I know the type of the method-invocation...now I need to check the parameters.
						//TODO: somehow get the declaration node of definition.

						return method_type;
					}
				}
			}

			return shptr<Type>();
		}
	} // namespace pe
} // namespace ast
