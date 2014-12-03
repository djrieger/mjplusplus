#include "../globals.hpp"
#include "Statement.hpp"

namespace ast
{
	Statement::Type Statement::getType() const
	{
		return Type::TYPE_SINGLE;
	}

	unsigned int Statement::countVariableDeclarations() const
	{
		return 0;
	}

	int Statement::setVariablePositions(shptr<std::map<std::string, int>> var2pos, int pos) const
	{
		return pos;
	}
}
