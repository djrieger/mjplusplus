#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>

#include "../globals.hpp"
#include "Statement.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	namespace stmt
	{
		class Block : public Statement
		{
			private:
				shptr<vec<shptr<Statement>>> block_statements;

			public:
				Block(shptr<vec<shptr<Statement> > > block_statements);
				virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
				virtual Statement::Type getType() const;
				virtual bool analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
				shptr<vec<shptr<Statement>>> getStatements() const;

				virtual unsigned int countVariableDeclarations() const;
				virtual int setVariablePositions(int) const;

				virtual void accept(ASTVisitor& visitor) const;
		};
	}
}

#endif
