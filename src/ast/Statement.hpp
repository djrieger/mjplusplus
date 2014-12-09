#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Node.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	namespace stmt
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
				virtual void accept(ASTVisitor& visitor) const;
				virtual int setVariablePositions(int) const;
		};
	}
}

#endif
