#ifndef IFSTATEMENT_HPP
#define IFSTATEMENT_HPP

#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	class IfStatement : public Statement
	{
		private:
			uptr<Expression> condition;
			uptr<Statement> thenStatement;
			uptr<Statement> elseStatement;

		public:
			IfStatement(uptr<Expression> condition, uptr<Statement> thenStatement);
			IfStatement(uptr<Expression> condition, uptr<Statement> thenStatement, uptr<Statement> elseStatement);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual Statement::Type getType() const;
	};
}

#endif
