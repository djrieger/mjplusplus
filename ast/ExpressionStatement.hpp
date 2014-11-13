#ifndef EXPRESIONSTATEMENT
#define EXPRESIONSTATEMENT

#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ExpressionStatement : public Statement
	{
		private:
			std::unique_ptr<Expression> expression;

		public:
			ExpressionStatement(std::unique_ptr<Expression>& expr);
			virtual std::string toString();
	};
}
#endif // EXPRESIONSTATEMENT
