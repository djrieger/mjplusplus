#include "Node.hpp"

namespace ast
{
	class ReturnStatement : public Statement
	{
		private:
			Expression& expression;

		public:
			ReturnStatement(Expression& expr);
	}
}