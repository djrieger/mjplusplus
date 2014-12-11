#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <vector>
#include "../globals.hpp"
#include "Node.hpp"
#include "ClassDeclaration.hpp"

namespace ast
{
	class ClassDeclaration;

	class Program : public Node
	{
		private:
			shptr<vec<shptr<ClassDeclaration>>> classes;

		public:
			Program(shptr<vec<shptr<ClassDeclaration>>> classes);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void collectDefinitions(semantic::SemanticAnalysis& sa) const;
			virtual void analyze(semantic::SemanticAnalysis& sa) const;
			void addPseudoClasses();
			virtual void accept(ASTVisitor& visitor) const;
			shptr<vec<shptr<ClassDeclaration>>> const getClasses() const;
	};
}

#endif // PROGRAM_HPP
