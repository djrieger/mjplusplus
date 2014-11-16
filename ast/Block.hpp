#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Statement.hpp"
#include <vector>

namespace ast
{
	class Block : public Statement
	{
		private:
			std::unique_ptr<std::vector<std::unique_ptr<Statement>>> block_statements;

		public:
			Block(std::unique_ptr<std::vector<std::unique_ptr<Statement>>>& block_statements);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual Statement::Type getType() const;
	};
}

#endif
