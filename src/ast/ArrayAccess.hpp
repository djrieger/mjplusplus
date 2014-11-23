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
			virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<Type> callingType) const;
			virtual bool lValueHelp() const;
	};
}

#endif
