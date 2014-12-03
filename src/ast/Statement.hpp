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

			/**
			 * @brief analyze a statement for semantic correctness, and recursivly analyze its children
			 * @return true iff all paths of this statement contsin a return statement
			 */
			virtual bool analyze(SemanticAnalysis& sa, shptr<SymbolTable> symboltable) const = 0;

			virtual unsigned int countVariableDeclarations() const;
			virtual int setVariablePositions(shptr<std::map<std::string, int>>, int) const;
	};
}

#endif
