#ifndef METHOD_DECLARATION_HPP
#define METHOD_DECLARATION_HPP

#include "../globals.hpp"
#include "ClassMember.hpp"
#include <vector>
#include "TypeIdent.hpp"
#include "Statement.hpp"
#include "Block.hpp"
#include "LVDStatement.hpp"

namespace ast
{
	class MethodDeclaration : public ClassMember
	{
		protected:
			shptr<TypeIdent> return_type_and_name;
			shptr<vec<shptr<TypeIdent>>> parameters;
			shptr<Block> block;

		public:
			MethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<Block> block);

			/*
			 * Prints this method including an optional "static" modifier (depending on isStatic)
			 */
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual std::string getName() const;
			shptr<vec<shptr<TypeIdent>>> getParameters() const;
			shptr<Type> getReturnType() const;
			virtual void collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const;
			virtual shptr<vec<shptr<ast::Type>>> collectParameters(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			virtual void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			shptr<Block> getBlock() const;
			unsigned int countVariableDeclarations() const;

			virtual void accept(ASTVisitor& visitor) const;
	};
}

#endif
