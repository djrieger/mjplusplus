#ifndef FIELDACCESS_HPP
#define FIELDACCESS_HPP

#include "PostfixOp.hpp"
#include "Ident.hpp"

namespace ast
{
	class FieldAccess : public PostfixOp
	{
		private:
			shptr<Ident> field_name;

		public:
			FieldAccess(shptr<Ident> field_name);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<Type> callingType) const;
	};
}

#endif
