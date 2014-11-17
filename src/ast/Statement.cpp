#include "../globals.hpp"
#include "Statement.hpp"

namespace ast
{
	Statement::Type Statement::getType() const
	{
		return Type::TYPE_SINGLE;
	}
}
