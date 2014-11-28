#ifndef METHOD_INVOCATION_BASE_HPP
#define METHOD_INVOCATION_BASE_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "MethodDeclaration.hpp"
#include "../util/MethodTable.hpp"
#include "Arguments.hpp"
#include "Ident.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"
#include "../util/symbol_table/symbol_table.hpp"

namespace ast
{
	class MethodInvocationBase
	{
		protected:
			shptr<const MethodDeclaration> callTarget;
			shptr<Arguments> arguments;
		public:
			shptr<ast::Type> performTypeChecks(shptr<ast::Ident> method_ident, MethodTable::MethodTableItem const& method_item, SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			MethodInvocationBase(shptr<Arguments> arguments);
	};
}

#endif