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
			MainMethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<Statement> block);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void collectParameters(SemanticAnalysis&, shptr<SymbolTable> symbolTable) const;
			virtual void collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
	};
}

#endif
