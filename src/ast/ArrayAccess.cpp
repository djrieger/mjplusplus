#include "../globals.hpp"
#include "ArrayAccess.hpp"

namespace ast
{
	ArrayAccess::ArrayAccess(uptr<Expression> access_offset) : access_offset(std::move(access_offset))
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
