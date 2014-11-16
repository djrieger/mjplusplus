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
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}
#endif // EXPRESIONSTATEMENT
