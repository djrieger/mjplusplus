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
			std::unique_ptr<Ident> method_name;
			std::unique_ptr<std::vector<std::unique_ptr<Expression>>> arguments;

		public:
			MethodInvocation(std::unique_ptr<Ident>& method_name, std::unique_ptr<std::vector<std::unique_ptr<Expression>>>& arguments);
			virtual std::string toString() const;
	};
}

#endif
