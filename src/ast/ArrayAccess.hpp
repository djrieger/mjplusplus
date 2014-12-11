#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include "Expression.hpp"
#include "PostfixOp.hpp"

namespace ast
{
	namespace po
	{
		class ArrayAccess : public PostfixOp
		{
			private:
				shptr<Expression> access_offset;

			public:
				ArrayAccess(shptr<Expression> access_offset);
				virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
				virtual shptr<Type> get_type(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable, shptr<Type> callingType) ;
				virtual bool lValueHelp() const;
				virtual void accept(ASTVisitor& visitor) const;
				shptr<Expression> getAccessOffset() const;
		};
	}
}

#endif
