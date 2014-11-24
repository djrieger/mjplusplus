#ifndef CLASSDECLARATION_HPP
#define CLASSDECLARATION_HPP

#include <vector>

#include "Ident.hpp"
#include "ClassMember.hpp"
#include "Node.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

class SemanticAnalysis;

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
			bool collectDefinitions(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			void analyze(SemanticAnalysis& sa) const;
			std::string getName() const;
			shptr<Ident> getIdent() const;
	};
}

#endif
