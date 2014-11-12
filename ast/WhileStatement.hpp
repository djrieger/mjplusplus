#ifndef WHILESTATEMENT_HPP
#define WHILESTATEMENT_HPP

#include "Node.hpp"

namespace ast
{
	class WhileStatement : public Statement
	{
		protected:
			Expression& condition;
			Statement& statement;

		public:
			WhileStatement(Expression& condition, Statement& statement);
	};
}

#endif