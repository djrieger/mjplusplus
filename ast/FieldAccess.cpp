#include "../globals.hpp"
#include "FieldAccess.hpp"

namespace ast
{
	FieldAccess::FieldAccess(std::unique_ptr<Ident>& field_name) : field_name(std::move(field_name))
	{
		;
	}

	void FieldAccess::toString(std::ostream& out, unsigned int indent, bool) const
	{
		out << '.';
		field_name->toString(out, indent);
	}
}
