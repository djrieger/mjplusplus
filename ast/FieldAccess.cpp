#include "FieldAccess.hpp"

namespace ast
{
	FieldAccess::FieldAccess(std::unique_ptr<Ident>& field_name) : field_name(std::move(field_name))
	{
		;
	}

	std::string FieldAccess::toString()
	{
		return '.' + field_name->toString();
	}
}
