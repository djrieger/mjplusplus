#ifndef EXPRESIONSTATEMENT
#define EXPRESIONSTATEMENT

#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ExpressionStatement : public Statement
	{
		private:
			shptr<Expression> expression;

		public:
			ExpressionStatement(shptr<Expression> expr);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}
#endif // EXPRESIONSTATEMENT
