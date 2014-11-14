#include "Block.hpp"

namespace ast
{
	Block::Block(std::unique_ptr<std::vector<std::unique_ptr<Statement>>>& block_statements)
		: block_statements(std::move(block_statements))
	{
		;
	}
	std::string Block::toString(unsigned int indent) const
	{
		if (block_statements->size() == 0)
			return "{ }";
		else
		{
			std::string r("{\n");

			for (auto& stamtement : *block_statements)
				r += stamtement->toString(indent);

			r += std::string(indent - 1, '\t') + "}\n";

			return r;
		}
	}
}
