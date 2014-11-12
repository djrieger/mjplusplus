#include "Node.hpp"

class IfStatement : public Statement
{
	protected:
		Expression& condition;
		Statement& thenStatement;

	public:
		IfStatement(Expression& condition, Statement& thenStatement);
}