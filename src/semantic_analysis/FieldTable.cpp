#include "FieldTable.hpp"

namespace semantic
{
	bool FieldTable::insertField(const std::string& fieldName, shptr<const ast::FieldDeclaration>& node,
	                             shptr<ast::Type>& type)
	{
		return fieldTable.insert({fieldName, {node, type}}).second;
	}

	std::unordered_map<std::string, FieldTable::FieldTableItem> const& FieldTable::getFieldTable() const
	{
		return this->fieldTable;
	}
}
