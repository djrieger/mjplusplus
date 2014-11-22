#ifndef METHOD_DECLARATION_HPP
#define METHOD_DECLARATION_HPP

#include "../globals.hpp"
#include "ClassMember.hpp"
#include <vector>
#include "TypeIdent.hpp"
#include "Statement.hpp"

namespace ast
{
	class MethodDeclaration : public ClassMember
	{
		protected:
			shptr<TypeIdent> return_type_and_name;
			shptr<vec<shptr<TypeIdent>>> parameters;
			shptr<Statement> block;

		public:
			MethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<Statement> block);

			/*
			 * Prints this method including an optional "static" modifier (depending on isStatic)
			 */
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual std::string getName() const;
			virtual void collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			virtual void collectParameters(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			virtual void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
	};
}

#endif
