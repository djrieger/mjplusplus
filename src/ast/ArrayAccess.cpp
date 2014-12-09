#include "../globals.hpp"
#include "ArrayAccess.hpp"

namespace ast
{
	namespace po
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

		shptr<Type> ArrayAccess::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<Type> callingType)
		{
			auto offsetType = access_offset->get_type(sa, symbolTable);

			if (offsetType)
			{
				if (offsetType->getPrimitiveType() == Type::Primitive_type::INT&&   offsetType->getDimension() == 0)
				{
					if (callingType->getDimension() > 0)
						return callingType->de_array();
					else
						sa.reportError("Trying to perform an array access on non-array type.", access_offset);
				}
				else
					sa.reportError("Trying to perform an array access with a non-int expression.", access_offset);
			}

			return shptr<Type>();
		}

		bool ArrayAccess::lValueHelp() const
		{
			return true;
		}

		void ArrayAccess::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<ArrayAccess const>(shared_from_this()));
		}

		shptr<Expression> ArrayAccess::getAccessOffset() const
		{
			return access_offset;
		}
	}
}
