#ifndef RETURNSTATEMENT_HPP
#define RETURNSTATEMENT_HPP

#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ReturnStatement : public Statement
	{
		private:
			std::unique_ptr<Expression> expression;

		public:
			ReturnStatement();
			ReturnStatement(std::unique_ptr<Expression>& expression);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
	};
}

#endif
