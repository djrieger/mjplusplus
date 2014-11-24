#include "../globals.hpp"
#include "FieldAccess.hpp"

namespace ast
{
	FieldAccess::FieldAccess(shptr<Ident> field_name) : field_name(field_name)
	{
		;
	}

	void FieldAccess::toString(std::ostream& out, unsigned int indent, bool) const
	{
		out << '.';
		field_name->toString(out, indent);
	}

	shptr<Type> FieldAccess::get_type(SemanticAnalysis& sa, shptr<SymbolTable>, shptr<Type> callingType) const
	{
		if (callingType->isClassType())
		{
			auto class_table = sa.getClassTable();
			auto class_it = class_table.find(callingType->getClassName());

			if (class_it != class_table.end())
			{
				auto& class_item = class_it->second;
				auto field_table = class_item.fieldTable->getFieldTable();
				auto field_it = field_table.find(field_name->getName());

				if (field_it != field_table.end())
				{
					auto field_item = field_it->second;
					return field_item.type;
				}
				else
				{
					sa.reportError(callingType->getName() + " has no field with the name " + field_name->getName(),
					               field_name);
				}
			}
			else
				sa.reportError("No such class: " + callingType->getClassName(), field_name);
		}
		else
			sa.reportError("Cannot access a field on a primitive or array type.", field_name);

		return shptr<ast::Type>();


		//		if (callingType->isClassType())
		//		{
		//			auto class_table = sa.getClassTable();
		//			auto class_item = class_table[callingType->getClassName()];

		//			auto fieldSymbol = Symbol::makeSymbol(field_name->getName());

		//			auto field_def = class_item.symbolTable->lookup(fieldSymbol);

		//			if (field_def)
		//				return field_def->getType();
		//			else
		//			{
		//				sa.reportError(callingType->getName() + " has no field with the name " + field_name->getName(),
		//				              field_name);
		//			}
		//		}
		//		else
		//			sa.reportError("Cannot invoke a field on a primitive or array type.", field_name);

		//		return shptr<ast::Type>();
	}

	bool FieldAccess::lValueHelp() const
	{
		return true;
	}
}
