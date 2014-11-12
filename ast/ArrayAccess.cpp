#include "ArrayAccess.hpp"

namespace ast
{
	ArrayAccess::ArrayAccess(Expression& access_offset) : access_offset(access_offset)
	{
		;
	}

	std::string ArrayAccess::toString()
	{
		return '[' + access_offset.toString() + ']';
	}
}

#endif
