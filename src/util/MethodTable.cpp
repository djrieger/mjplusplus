#include "MethodTable.hpp"

bool MethodTable::insertMethod(const std::string& methodName, shptr<ast::MethodDeclaration>& node,
                               shptr<ast::Type>&  returnType, shptr<vec<shptr<ast::Type>>>& parameterTypes)
{
	return methodTable.insert({methodName, {node, returnType, parameterTypes}}).second;
}

std::unordered_map<std::string, MethodTable::MethodTableItem> const& MethodTable::getMethodTable() const
{
	return this->methodTable;
}
