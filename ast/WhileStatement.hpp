#ifndef WHILESTATEMENT_HPP
#define WHILESTATEMENT_HPP

#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	class WhileStatement : public Statement
	{
		protected:
			Expression& condition;
			Statement& statement;

		public:
			WhileStatement(Expression& condition, Statement& statement);
			virtual std::string toString();
	};
}

#endif