#ifndef WHILESTATEMENT_HPP
#define WHILESTATEMENT_HPP

#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	class WhileStatement : public Statement
	{
		protected:
			std::unique_ptr<Expression> condition;
			std::unique_ptr<Statement> statement;

		public:
			WhileStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& statement);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
	};
}

#endif
