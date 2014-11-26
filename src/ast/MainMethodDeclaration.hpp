#ifndef MAIN_METHOD_DECLARATION_HPP
#define MAIN_METHOD_DECLARATION_HPP

#include "../globals.hpp"
#include "MethodDeclaration.hpp"
#include "Statement.hpp"

namespace ast
{
	class MainMethodDeclaration : public MethodDeclaration
	{
		public:
			MainMethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<Block> block);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const;
			virtual shptr<vec<shptr<ast::Type>>> collectParameters(SemanticAnalysis&, shptr<SymbolTable> symbolTable) const;
			void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
	};
}

#endif
