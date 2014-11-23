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

	bool ArrayAccess::isValidType(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
	{
		return access_offset->get_type(sa, symbolTable)->getPrimitiveType() == Type::Primitive_type::INT;
	}
}
