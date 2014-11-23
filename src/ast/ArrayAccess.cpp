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

	shptr<Type> ArrayAccess::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<Type> callingType) const
	{
		auto offsetType = access_offset->get_type(sa, symbolTable);

		if (offsetType)
		{
			if (offsetType->getPrimitiveType() == Type::Primitive_type::INT&&  offsetType->getDimension() == 0)
			{
				if (callingType->getDimension() > 0)
					return callingType->de_array();
				else
					sa.printError("Trying to perform an array access on non-array type.");
			}
			else
				sa.printError("Trying to perform an array access with a non-int expression.");
		}

		return shptr<Type>();
	}

	bool ArrayAccess::lValueHelp() const
	{
		return true;
	}
}
