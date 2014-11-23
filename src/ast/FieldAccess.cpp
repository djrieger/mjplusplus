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
			auto class_item = class_table[callingType->getClassName()];

			auto fieldSymbol = Symbol::makeSymbol(field_name->getName());

			auto field_def = class_item.symbolTable->lookup(fieldSymbol);

			if (field_def)
				return field_def->getType();
			else
			{
				sa.printError(callingType->getName() + " has no field with the name " + field_name->getName(),
				              field_name);
			}
		}
		else
			sa.printError("Cannot invoke a field on a primitive or array type.", field_name);

		return shptr<ast::Type>();
	}

	bool FieldAccess::lValueHelp() const
	{
		return true;
	}
}
