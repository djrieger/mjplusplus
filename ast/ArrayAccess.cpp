#include "ArrayAccess.hpp"

namespace ast
{
	ArrayAccess::ArrayAccess(std::unique_ptr<Expression>& access_offset) : access_offset(std::move(access_offset))
	{
		;
	}

	std::string ArrayAccess::toString(unsigned int indent) const
	{
		return '[' + access_offset->toString(indent) + ']';
	}
}
