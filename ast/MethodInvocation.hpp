#ifndef METHOD_INVOCATION_HPP
#define METHOD_INVOCATION_HPP

#include "PostfixOp.hpp"
#include "Ident.hpp"
#include <vector>
#include "Expression.hpp"

namespace ast
{
	class MethodInvocation : public PostfixOp
	{
		private:
			Ident& method_name;
			std::vector<Expression> arguments;

		public:
			MethodInvocation(Ident& method_name, std::vector<Expression> arguments);
            virtual std::string toString();
    }
}

#endif
