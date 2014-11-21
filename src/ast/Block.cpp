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

	void Block::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
	{
		symbolTable->enterScope();

		for (auto& stmt : *block_statements)
			stmt->analyze(sa, symbolTable);

		symbolTable->leaveScope();
	}
}
