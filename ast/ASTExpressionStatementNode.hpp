#include "Node.hpp"

namespace ast
{
	class ExpressionStatement : public Statement
	{
		private:
			Expression& expression;

		public:
			ExpressionStatement(Expression& expr);
	}
}