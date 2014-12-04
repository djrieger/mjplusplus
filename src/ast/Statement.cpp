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

	void Statement::accept(ASTVisitor& visitor) const
	{
		visitor.visit(std::static_pointer_cast<Statement const>(shared_from_this()));
	}

	int Statement::setVariablePositions(int pos) const
	{
		return pos;
	}
}
