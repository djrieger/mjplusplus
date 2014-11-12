#ifndef IFELSESTATEMENT_H
#define IFELSESTATEMENT_H

#include "Node.hpp"
#include "IfStatement.hpp"

namespace ast
{
	class IfElseStatement : public IfStatement
	{
		protected:
			Statement& elseStatement;

		public:
			IfElseStatement(Expression& condition, Statement& thenStatement, Statement& elseStatement);
			std::string toString();
	};
}

#endif
