#include "ArrayAccess.hpp"

namespace ast
{
	ArrayAccess::ArrayAccess(std::unique_ptr<Expression>& access_offset) : access_offset(std::move(access_offset))
	{
		;
	}

	void ArrayAccess::toString(std::ostream& out, unsigned int indent, bool special) const
	{
		out << '[';
		access_offset->toString(out, indent, true);
		out << ']';
	}
}
