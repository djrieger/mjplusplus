#ifndef FIELD_TABLE_HPP
#define FIELD_TABLE_HPP

#include <unordered_map>
#include "../globals.hpp"
#include "../ast/Type.hpp"

namespace ast
{
	class FieldDeclaration;
}


namespace semantic
{
	class FieldTable
	{
		public:
			struct FieldTableItem
			{
				shptr<ast::FieldDeclaration const> fieldNode;
				shptr<ast::Type> type;
			};

		private:
			std::unordered_map<std::string, FieldTableItem> fieldTable;

		public:
			bool insertField(const std::string& fieldName, shptr<const ast::FieldDeclaration>& node,
			                 shptr<ast::Type>& type);
			std::unordered_map<std::string, FieldTableItem> const& getFieldTable() const;
	};
}

#endif
