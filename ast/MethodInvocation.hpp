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
			std::unique_ptr<Ident> method_name;
			std::unique_ptr<Arguments> arguments;

		public:
			MethodInvocation(std::unique_ptr<Ident>& method_name, std::unique_ptr<Arguments>& arguments);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
