#ifndef CLASSDECLARATION_HPP
#define CLASSDECLARATION_HPP

#include <vector>

#include "Ident.hpp"
#include "ClassMember.hpp"
#include "Node.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	class ClassMember;

	class ClassDeclaration : public Node
	{
		private:
			shptr<Ident> class_name;
			shptr<vec<shptr<ClassMember>>> members;

		public:
			ClassDeclaration(shptr<Ident> class_name, shptr<vec<shptr<ClassMember>>> members);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			/*
			 * @return whether this class contains a main method
			 */
			bool collectDefinitions(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
			void analyze(semantic::SemanticAnalysis& sa) const;
			std::string getName() const;
			shptr<Ident> getIdent() const;
			virtual void accept(ASTVisitor& visitor) const;
			shptr<vec<shptr<ClassMember>>> getMembers() const;
	};
}

#endif
