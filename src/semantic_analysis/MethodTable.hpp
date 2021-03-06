#ifndef METHOD_TABLE_HPP
#define METHOD_TABLE_HPP

#include <unordered_map>
#include "../globals.hpp"
#include "../ast/Type.hpp"

namespace ast
{
	class MethodDeclaration;
}

namespace semantic
{
	class MethodTable
	{
		public:
			struct MethodTableItem
			{
				shptr<const ast::MethodDeclaration> methodNode;
				shptr<ast::Type>  returnType;
				shptr<vec<shptr<ast::Type>>> parameterTypes;
			};

		private:
			std::unordered_map<std::string, MethodTableItem> methodTable;

		public:
			bool insertMethod(const std::string& methodName, shptr<const ast::MethodDeclaration>& node,
			                  shptr<ast::Type>& returnType, shptr<vec<shptr<ast::Type>>>& parameterTypes);
			std::unordered_map<std::string, MethodTableItem> const& getMethodTable() const;
	};
}

#endif
