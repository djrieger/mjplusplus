#include "Node.hpp"

class ReturnStatement : public Statement
{
	private:
		Expression& expression;

	public:
		ReturnStatement(Expression& expr);
}