#ifndef IFSTATEMENT_H
#define IFSTATEMENT_H

#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	class IfStatement : public Statement
	{
		protected:
			Expression& condition;
			Statement& thenStatement;

		public:
			IfStatement(Expression& condition, Statement& thenStatement);
			virtual std::string toString();
	};
}

#endif
