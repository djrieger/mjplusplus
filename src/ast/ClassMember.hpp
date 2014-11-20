#ifndef CLASSMEMBER_HPP
#define CLASSMEMBER_HPP

#include "Node.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	class ClassMember : public Node
	{

		public:
			virtual std::string getName() const = 0;
			virtual void collectDefinition(shptr<SemanticAnalysis> sa, shptr<SymbolTable> symbolTable) const = 0;
	};
}

#endif
