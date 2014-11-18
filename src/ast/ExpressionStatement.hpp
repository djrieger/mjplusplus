#ifndef EXPRESIONSTATEMENT
#define EXPRESIONSTATEMENT

#include "Statement.hpp"
#include "Expression.hpp"

namespace ast
{
	class ExpressionStatement : public Statement
	{
		private:
			uptr<Expression> expression;

		public:
			ExpressionStatement(uptr<Expression>& expr);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}
#endif // EXPRESIONSTATEMENT
