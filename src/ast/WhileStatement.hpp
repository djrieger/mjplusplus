#ifndef WHILESTATEMENT_HPP
#define WHILESTATEMENT_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	class WhileStatement : public Statement
	{
		protected:
			uptr<Expression> condition;
			uptr<Statement> statement;

		public:
			WhileStatement(uptr<Expression> condition, uptr<Statement> statement);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
	};
}

#endif
