#include "../globals.hpp"
#include "FieldAccess.hpp"

namespace ast
{
	namespace po
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

		shptr<Ident> FieldAccess::getFieldName() const
		{
			return field_name;
		}

		shptr<Type> FieldAccess::get_type(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable>, shptr<Type> callingType)
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
						declaration = field_item.fieldNode;
						return field_item.type;
					}
					else
					{
						sa.reportError("$type{" + callingType->getName() + "} has no field with the name $ident{" + field_name->getName() + "}",
						               field_name);
					}
				}
				else
					sa.reportError("No such class: $type{" + callingType->getClassName() + "}", field_name);
			}
			else
				sa.reportError("Cannot access a field on a primitive or array type.", field_name);

			return shptr<ast::Type>();
		}

		bool FieldAccess::lValueHelp() const
		{
			return true;
		}

		void FieldAccess::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<FieldAccess const>(shared_from_this()));
		}
	}
}
