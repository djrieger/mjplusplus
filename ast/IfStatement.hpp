#ifndef IFSTATEMENT_H
#define IFSTATEMENT_H

#include "Expression.hpp"
#include "Statement.hpp"

namespace ast
{
	class IfStatement : public Statement
	{
		protected:
			std::unique_ptr<Expression> condition;
			std::unique_ptr<Statement> thenStatement;
			std::unique_ptr<Statement> elseStatement;

		public:
			IfStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& thenStatement);
			IfStatement(std::unique_ptr<Expression>& condition, std::unique_ptr<Statement>& thenStatement, std::unique_ptr<Statement>& elseStatement);
			virtual std::string toString();
	};
}

#endif
