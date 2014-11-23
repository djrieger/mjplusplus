#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include "Expression.hpp"
#include "PostfixOp.hpp"

namespace ast
{
	class ArrayAccess : public PostfixOp
	{
		private:
			shptr<Expression> access_offset;

		public:
			ArrayAccess(shptr<Expression> access_offset);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			/*
			 * checks if the expression stored in access_offset is of type INT
			 */
			bool isValidType(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
	};
}

#endif
