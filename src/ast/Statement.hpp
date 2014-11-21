#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Node.hpp"

#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	class Statement : public Node
	{
		public:
			enum Type
			{
				TYPE_SINGLE,
				TYPE_BLOCK,
				TYPE_IF
			};

			virtual Type getType() const;
			virtual void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symboltable) const = 0;
	};
}

#endif
