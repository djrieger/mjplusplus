#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "Node.hpp"
#include "Type.hpp"
#include "../util/symbol_table/symbol_table.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	class Expression : public Node
	{
		public:
			virtual bool check_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			virtual Type get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
	};
}

#endif
