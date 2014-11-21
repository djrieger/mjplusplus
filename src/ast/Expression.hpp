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
			/**
			* Idea:
			* When getting the type of an Expression, the Expression checks if it's own 
			* and it's children Expressions type definitions are valid and then returns
			* it's type. If something is invalid, an error is printed in the Expression
			* that is invalid and an empty pointer is returned. 
			*/
			virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const = 0;
	};
}

#endif
