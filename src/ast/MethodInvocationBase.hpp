#ifndef METHOD_INVOCATION_BASE_HPP
#define METHOD_INVOCATION_BASE_HPP

#include "../globals.hpp"
#include "../semantic_analysis/MethodTable.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"
#include "../semantic_analysis/symbol_table/symbol_table.hpp"

namespace ast
{
	class Ident;
	class Type;
	class MethodDeclaration;
	class Arguments;

	class MethodInvocationBase
	{
		protected:
			shptr<Ident> method_name;
			mutable shptr<MethodDeclaration const> declaration;
			shptr<Arguments> arguments;
			MethodInvocationBase(shptr<Ident> method_name, shptr<Arguments> arguments);
		public:
			shptr<ast::Type> performTypeChecks(shptr<ast::Ident> method_ident, semantic::MethodTable::MethodTableItem const& method_item, semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
			shptr<MethodDeclaration const> getDeclaration() const;
			virtual std::string const& getIdentifier() const;
			virtual shptr<Arguments const> getArguments() const;
	};
}

#endif
