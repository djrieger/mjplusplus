#ifndef IFSTATEMENT_H
#define IFSTATEMENT_H

#include "Node.hpp"

namespace ast
{
	class IfStatement : public Statement
	{
		protected:
			Expression& condition;
			Statement& thenStatement;

		public:
			IfStatement(Expression& condition, Statement& thenStatement);
	};
}

#endif
