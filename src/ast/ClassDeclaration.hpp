#ifndef CLASSDECLARATION_HPP
#define CLASSDECLARATION_HPP

#include <vector>

#include "Ident.hpp"
#include "ClassMember.hpp"
#include "Node.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	class ClassDeclaration : public Node
	{
		private:
			shptr<Ident> class_name;
			shptr<vec<shptr<ClassMember>>> members;

		public:
			ClassDeclaration(shptr<Ident> class_name, shptr<vec<shptr<ClassMember>>> members);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			void collectDefinitions(shptr<SemanticAnalysis> sa, shptr<SymbolTable> symbolTable) const;
			std::string getName() const;
	};
}

#endif
