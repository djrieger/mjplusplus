#include "../globals.hpp"
#include "Block.hpp"

namespace ast
{
	Block::Block(shptr<vec<shptr<Statement>>> block_statements)
		: block_statements(block_statements)
	{
		;
	}
	void Block::toString(std::ostream& out, unsigned int indent, bool special) const
	{
		unsigned int real_indent = (indent > ~indent ? ~indent : indent);

		if (!special)
			out << std::string(real_indent, '\t');

		out << "{\n";

		for (auto& stamtement : *block_statements)
			stamtement->toString(out, real_indent + 1);

		out << std::string(real_indent, '\t') << "}";
		out << (real_indent != indent ? ' ' : '\n');
	}

	Statement::Type Block::getType() const
	{
		return Type::TYPE_BLOCK;
	}

	bool Block::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
	{
		symbolTable->enterScope();

		bool returns = false;

		//optimization: analyze all code for semantic correctness, then remove unreachable code
		auto it = block_statements->begin();
		auto cut_from = block_statements->end();

		for (; it != block_statements->end(); it++)
		{
			bool stmt_returns = (*it)->analyze(sa, symbolTable);

			if (stmt_returns && !returns)
			{
				cut_from = it + 1;
				returns = true;
			}
		}

		block_statements->erase(cut_from, block_statements->end());
		/* unoptimized version
		for (auto& stmt : *block_statements)
			returns = stmt->analyze(sa, symbolTable) || returns;
		*/

		symbolTable->leaveScope();
		return returns;
	}

	shptr<vec<shptr<Statement>>> ast::Block::getStatements() const
	{
		return block_statements;
	}

	unsigned int ast::Block::countVariableDeclarations() const
	{
		// TODO optimize with reduce, fold, lambda or whatever
		unsigned int variableDeclarations = 0;
		for (auto &blockStatement: *block_statements) {
			variableDeclarations += blockStatement->countVariableDeclarations();
		}
		return variableDeclarations;
	}
}
