#include "Block.hpp"

namespace ast
{
	Block::Block(std::unique_ptr<std::vector<std::unique_ptr<Statement>>>& block_statements)
		: block_statements(std::move(block_statements))
	{
		;
	}
	void Block::toString(std::ostream& out, unsigned int indent, bool special) const
	{
		if (block_statements->size() == 0)
			out << "{ }";
		else
		{
			out << "{\n";

			for (auto& stamtement : *block_statements)
				stamtement->toString(out, indent);

			out << std::string(indent - 1, '\t') << "}";
			out << (special ? ' ' : '\n');
		}
	}

	Statement::Type Block::getType() const
	{
		return Type::TYPE_BLOCK;
	}
}
