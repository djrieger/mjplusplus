#include <string>
#include <limits>

#include "../globals.hpp"
#include "PrimaryExpression.hpp"

namespace ast
{
	namespace pe
	{
		PrimaryExpression::PrimaryExpression(source_position_t position) : Expression(position)
		{
			;
		}

		bool PrimaryExpression::standalone() const
		{
			return false;
		}

		std::pair<bool, bool> PrimaryExpression::constBool() const
		{
			return {false, false};
		}

		Bool::Bool(bool value, source_position_t position) : PrimaryExpression(position), value(value)
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


		std::pair<bool, bool> Bool::constBool() const
		{
			return {true, value};
		}

		void Bool::accept(ASTVisitor& visitor) const
		{
			visitor.visit(shared_from_this());
		}


		Ident::Ident(shptr<ast::Ident> identifier) : PrimaryExpression(identifier->getPosition()), identifier(identifier)
		{
		}

		void Ident::toString(std::ostream& out, unsigned int ident, bool) const
		{
			identifier->toString(out, ident);
		}

		shptr<Type> Ident::get_type(SemanticAnalysis& sa, shptr<SymbolTable>) const
		{
			// lookup in (local) symbol table
			auto system_type = std::shared_ptr<Type>();
			auto ident_symbol = Symbol::makeSymbol(identifier->getName());

			if (ident_symbol)
			{
				shptr<Definition> ident_def = ident_symbol->getCurrentDefinition();

				if (ident_def)
				{
					shptr<Type> ident_type = ident_def->getType();

					if (sa.isTypeDefined(ident_type))
					{
						if (ident_type->getName() != "$System")
							return ident_type;
						else
							system_type = ident_type;
					}
				}

				// else do lookup in field table
			}

			// lookup in field table
			auto class_table = sa.getClassTable();
			auto this_symbol = Symbol::makeSymbol("this");
			auto definition = this_symbol->getCurrentDefinition();

			if (!definition)
			{
				// in a static function aka main
				if (ident_symbol)
				{
					shptr<Definition> ident_def = ident_symbol->getCurrentDefinition();

					if (ident_def)
					{
						shptr<Type> ident_type = ident_def->getType();

						if (sa.isTypeDefined(ident_type))
							return ident_type;
					}
				}

				if (system_type)
					return system_type;
				else
					sa.reportError("Symbol not defined!", identifier);
			}
			else
			{
				// in a non-static function
				auto class_type = definition->getType();
				auto class_item  = class_table[class_type->getClassName()];

				auto field_table = class_item.fieldTable->getFieldTable();
				auto field_it = field_table.find(identifier->getName());

				if (field_it != field_table.end())
				{
					auto field_item = field_it->second;
					return field_item.type;
				}
				else
				{
					if (system_type)
						return system_type;
					else
						sa.reportError("$type{" + class_type->getName() + "} has no field named $ident{" + identifier->getName() + "}",
						               identifier);
				}
			}

			return shptr<ast::Type>();
		}

		bool Ident::isLValue() const
		{
			return true;
		}

		void Ident::accept(ASTVisitor& visitor) const
		{
			visitor.visit(shared_from_this());
		}

		Object::Object(Object_Type object_type, source_position_t position) : PrimaryExpression(position), object_type(object_type)
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

		void Object::accept(ASTVisitor& visitor) const
		{
			visitor.visit(shared_from_this());
		}

		Integer::Integer(std::string const& string_value, source_position_t position)
			: PrimaryExpression(position), string_value(string_value)
		{

		}

		shptr<Type>Integer::get_type(SemanticAnalysis& sa, shptr<SymbolTable>) const
		{
			unsigned long long value;

			try
			{
				value = std::stoull(string_value);
			}
			catch (std::out_of_range)
			{
				value = std::numeric_limits<unsigned long long>::max();
			}

			if (value > 2147483648ULL)
			{
				sa.reportError("Integer literal too large", this->getPosition());
				return std::make_shared<Type>(Type::INT);
			}
			else if (value == 2147483648ULL)
				return std::make_shared<Type>(Type::INT_NEG_ONLY);
			else
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

		void Integer::accept(ASTVisitor& visitor) const
		{
			visitor.visit(shared_from_this());
		}

		NewArrayExpression::NewArrayExpression(shptr<Type> type, shptr<Expression> expr) :
			PrimaryExpression(expr->getPosition()),
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
					else
						sa.reportError("Type $type{" + type->getName() + "} not defined!", expr);

				}
				else
					sa.reportError("Array size needs to be an integer type.", expr);
			}

			return shptr<Type>();
		}


		void NewArrayExpression::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << "(new ";
			type->toString(out, indent, expr);
			out << ')';
		}

		bool NewArrayExpression::isLValue() const
		{
			return false;
		}

		void NewArrayExpression::accept(ASTVisitor& visitor) const
		{
			visitor.visit(shared_from_this());
		}

		NewObjectExpression::NewObjectExpression(shptr<ast::Ident> identifier) : PrimaryExpression(identifier->getPosition()), identifier(identifier)
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
			{
				sa.reportError("Type $type{" + type->getName() + "} not defined!", identifier);
				return shptr<Type>();
			}
		}

		bool NewObjectExpression::isLValue() const
		{
			return false;
		}

		bool NewObjectExpression::standalone() const
		{
			return true;
		}

		void NewObjectExpression::accept(ASTVisitor& visitor) const
		{
			visitor.visit(shared_from_this());
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
				sa.reportError("Symbol not defined!", this->identifier);
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
						if (!invType || !(*decType == *invType ||
						                  (decType->isRefType() && invType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE)))
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
						sa.reportError("Arguments do not match parameter types.", identifier);
				}
				else
					sa.reportError("Wrong number of arguments.", identifier);
			}
			else
			{
				sa.reportError("$type{" + class_type->getName() + "} has no method named $ident{" + identifier->getName() + "}",
				               identifier);
			}

			return shptr<ast::Type>();
		}

		bool MethodInvocation::isLValue() const
		{
			return false;
		}

		bool MethodInvocation::standalone() const
		{
			return true;
		}

		void MethodInvocation::accept(ASTVisitor& visitor) const
		{
			visitor.visit(shared_from_this());
		}

	} // namespace pe
} // namespace ast
