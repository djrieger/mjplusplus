#include "Node.hpp"

namespace ast
{
	class IfElseStatement : public IfStatement
	{
		protected:
			Statement& elseStatement;

		public:
			IfElseStatement(Expression& condition, Statement& thenStatement, Statement& elseStatement);
	}
}