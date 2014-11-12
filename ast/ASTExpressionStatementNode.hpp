#include "Node.hpp"

class ExpressionStatement : public Statement
{
	private:
		Expression& expression;

	public:
		ExpressionStatement(Expression& expr);
}