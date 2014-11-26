#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "../globals.hpp"
#include "Statement.hpp"
#include <vector>

namespace ast
{
	class Block : public Statement
	{
		private:
			shptr<vec<shptr<Statement>>> block_statements;

		public:
			Block(shptr<vec<shptr<Statement> > > block_statements);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual Statement::Type getType() const;
			virtual bool analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			shptr<vec<shptr<Statement>>> getStatements() const;

			virtual unsigned int countVariableDeclarations() const;
	};
}

#endif
