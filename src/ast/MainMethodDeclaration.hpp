#ifndef MAIN_METHOD_DECLARATION_HPP
#define MAIN_METHOD_DECLARATION_HPP

#include "../globals.hpp"
#include "MethodDeclaration.hpp"
#include "Statement.hpp"

namespace ast
{
	class  MainMethodDeclaration : public MethodDeclaration
	{
		public:
			MainMethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<stmt::Block> block);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void collectDefinition(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable, std::string const& class_name) const;
			virtual shptr<vec<shptr<ast::Type>>> collectParameters(semantic::SemanticAnalysis&, shptr<semantic::symbol::SymbolTable> symbolTable) const;
			void analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
			virtual void accept(ASTVisitor& visitor) const;
			virtual std::string mangle() const;
			virtual unsigned int countVariableDeclarations() const;
			virtual void createVariablePositions() const;
	};
}

#endif
