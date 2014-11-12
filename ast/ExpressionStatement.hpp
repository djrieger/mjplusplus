#ifndef EXPRESIONSTATEMENT
#define EXPRESIONSTATEMENT

#include "Statement.hpp"

namespace ast
{
	class ExpressionStatement : public Statement
	{
		private:
			Expression& expression;

		public:
			ExpressionStatement(Expression& expr);
	};
}
#endif // EXPRESIONSTATEMENT