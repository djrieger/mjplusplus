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

		bool Bool::isLValue() const
		{
			return false;
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
					sa.printError("No current definition for " + identifier->getName(), identifier);
			}

			return shptr<Type>();
		}

		bool Ident::isLValue() const
		{
			return true;
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
				return std::make_shared<Type>(Type::Primitive_type::NULL_TYPE);

			return shptr<Type>();
		}

		bool Object::isLValue() const
		{
			return false;
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

		bool Integer::isLValue() const
		{
			return false;
		}

		NewArrayExpression::NewArrayExpression(shptr<Type> type, shptr<Expression> expr) :
			type(type),
			expr(expr)
		{

		}

		shptr<Type>NewArrayExpression::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			shptr<Type> child_type = expr->get_type(sa, symbolTable);

			if (child_type)
			{
				if (child_type->isInteger())
				{
					if (sa.isTypeDefined(type))
						return type;
				}
				else
					sa.printError("Array size needs to be an integer type.");
			}

			return shptr<Type>();
		}


		void NewArrayExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			type->toString(out, indent, &expr);
			out << ')';
		}

		bool NewArrayExpression::isLValue() const
		{
			return false;
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

		bool NewObjectExpression::isLValue() const
		{
			return false;
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

		shptr<Type> MethodInvocation::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{

			auto class_table = sa.getClassTable();
			auto this_symbol = Symbol::makeSymbol("this");
			auto definition = this_symbol->getCurrentDefinition();

			if (!definition)
			{
				sa.printError("Symbol not defined!");
				return shptr<ast::Type>();
			}

			auto class_type = definition->getType();
			auto class_item  = class_table[class_type->getClassName()];

			auto method_table = class_item.methodTable->getMethodTable();
			auto method_it = method_table.find(identifier->getName());

			if (method_it != method_table.end())
			{
				auto method_item = method_it->second;
				shptr<vec<shptr<ast::Type>>> declarationTypes = method_item.parameterTypes;
				shptr<vec<shptr<ast::Expression>>> invokedExpressions = arguments->getArgumentTypes();

				int decSize = declarationTypes->size();
				int invSize = invokedExpressions->size();

				if (decSize == invSize)
				{
					bool validArguments = true;
					auto decIt = declarationTypes->begin();
					auto invIt = invokedExpressions->begin();

					for (int i = 0; i < invSize; i++)
					{
						auto decType = *decIt;
						auto invType = (*invIt)->get_type(sa, symbolTable);

						//TODO: check if invType is non-empty pointer
						if (!invType || *decType != *invType)
						{
							validArguments = false;
							break;
						}

						decIt++;
						invIt++;
					}

					if (validArguments)
						return method_item.returnType;
					else
						sa.printError("The arguments do not match the parameter types.", identifier);
				}
				else
					sa.printError("Wrong number of arguments.", identifier);
			}
			else
			{
				sa.printError(class_type->getName() + " has no method with the name " + identifier->getName(),
				              identifier);
			}

			return shptr<ast::Type>();
		}

		bool MethodInvocation::isLValue() const
		{
			return false;
		}
	} // namespace pe
} // namespace ast
