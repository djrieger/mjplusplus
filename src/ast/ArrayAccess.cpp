#include "../globals.hpp"
#include "ArrayAccess.hpp"

namespace ast
{
	ArrayAccess::ArrayAccess(shptr<Expression> access_offset) : access_offset(access_offset)
	{
		;
	}

	void ArrayAccess::toString(std::ostream& out, unsigned int indent, bool) const
	{
		out << '[';
		access_offset->toString(out, indent, true);
		out << ']';
	}
}
