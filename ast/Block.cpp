#include "Block.hpp"

namespace ast
{
	Block::Block(std::unique_ptr<std::vector<std::unique_ptr<Statement>>>& block_statements)
		: block_statements(std::move(block_statements))
	{
		;
	}
	std::string Block::toString() const
	{
		std::string r("{");

		for (auto it = block_statements->begin(); it != block_statements->end(); it++)
			r += (**it).toString();

		r += '}';

		return r;
	}
}
