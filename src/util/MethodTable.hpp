#ifndef METHOD_TABLE_HPP
#define METHOD_TABLE_HPP

#include <unordered_map>
#include "../globals.hpp"
#include "../ast/MethodDeclaration.hpp"
#include "../ast/Type.hpp"

class MethodTable
{
	public:
		struct MethodTableItem
		{
			shptr<ast::MethodDeclaration> methodNode;
			shptr<ast::Type>  returnType;
			shptr<vec<shptr<ast::Type>>> parameterTypes;
		};

	private:
		std::unordered_map<std::string, MethodTableItem> methodTable;

	public:
		bool insertMethod(const std::string& methodName, shptr<ast::MethodDeclaration>& node,
		                  shptr<ast::Type>&  returnType, shptr<vec<shptr<ast::Type>>>& parameterTypes);
		std::unordered_map<std::string, MethodTableItem> const& getMethodTable() const;
};
#endif
