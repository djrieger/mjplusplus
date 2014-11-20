#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <vector>
#include "../globals.hpp"
#include "Node.hpp"
#include "ClassDeclaration.hpp"

class SemanticAnalysis;

namespace ast
{
	class Program : public Node
	{
		private:
			shptr<vec<shptr<ClassDeclaration>>> classes;

		public:
			Program(shptr<vec<shptr<ClassDeclaration>>> classes);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void collectDefinitions(shptr<SemanticAnalysis> sa) const;
	};

}

#endif // PROGRAM_HPP
