#ifndef EXPRESIONSTATEMENT
#define EXPRESIONSTATEMENT

#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ExpressionStatement : public Statement
	{
		private:
			Expression& expression;

		public:
			ExpressionStatement(Expression& expr);
			virtual std::string toString();
	};
}
#endif // EXPRESIONSTATEMENT