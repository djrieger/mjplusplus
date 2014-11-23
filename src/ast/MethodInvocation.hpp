#ifndef METHOD_INVOCATION_HPP
#define METHOD_INVOCATION_HPP

#include "../globals.hpp"
#include <vector>
#include "PostfixOp.hpp"
#include "Ident.hpp"
#include "Expression.hpp"
#include "Arguments.hpp"

namespace ast
{
	class MethodInvocation : public PostfixOp
	{
		private:
			shptr<Ident> method_name;
			shptr<Arguments> arguments;

		public:
			MethodInvocation(shptr<Ident> method_name, shptr<Arguments> arguments);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<Type> callingType) const;
			virtual bool lValueHelp() const;
	};
}

#endif
