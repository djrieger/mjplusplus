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
}
