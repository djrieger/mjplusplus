#ifndef BLCOK_HPP
#define BLOCK_HPP

#include "Statement.hpp"

namespace ast
{
	class Block : public Statement
	{
		private:
			std::unique_ptr<std::vector<std::unique_ptr<Statement>>> block_statements;

		public:
			Block(std::unique_ptr<std::vector<std::unique_ptr<Statement>>>& block_statements);
			virtual std::string toString();
	};
}

#endif
