#ifndef RETURNSTATEMENT_HPP
#define RETURNSTATEMENT_HPP

#include "../globals.hpp"
#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ReturnStatement : public Statement
	{
		private:
			uptr<Expression> expression;

		public:
			ReturnStatement();
			ReturnStatement(uptr<Expression> expression);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
