#include "FieldAccess.hpp"

namespace ast
{
	FieldAccess::FieldAccess(Ident& field_name) : field_name(field_name)
	{
		;
	}

	std::string FieldAccess::toString()
	{
		return '.' + field_name.toString();
	}
}
