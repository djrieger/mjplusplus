#ifndef METHOD_INVOCATION_HPP
#define METHOD_INVOCATION_HPP

#include "../globals.hpp"
#include <vector>
#include "PostfixOp.hpp"
#include "Ident.hpp"
#include "Expression.hpp"
#include "Arguments.hpp"
#include "MethodInvocationBase.hpp"

namespace ast
{
	class MethodInvocation : public PostfixOp, public MethodInvocationBase
	{
		private:
			shptr<Ident> method_name;
		public:
			MethodInvocation(shptr<Ident> method_name, shptr<Arguments> arguments);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			/*
			 * Determines if this method call is valid. If it is, this also sets callTarget to the target of this MethodInvocation.
			 *
			 * Validity checks performed:
			 *  - Is callingType a class type, not an array or a primitive type
			 *  - Does the class referenced in callingType exist
			 *  - Does a method with name method_name exist in the referenced class
			 *  - Do number and types of supplied arguments match the signature of the referenced method
			 *
			 * @return	The target method's return type if the call is valid, an empty shared pointer otherwise
			 */
			virtual shptr<Type> get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<Type> callingType);
			virtual bool lValueHelp() const;
			virtual void accept(ASTVisitor& visitor) const;
	};
}

#endif
