#ifndef RETURNSTATEMENT_HPP
#define RETURNSTATEMENT_HPP

#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ReturnStatement : public Statement
	{
		private:
			Expression& expression;

		public:
			ReturnStatement(Expression& expr);
			virtual std::string toString();
	};
}

#endif