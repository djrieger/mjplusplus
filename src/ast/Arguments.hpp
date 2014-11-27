#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Expression.hpp"

#include <vector>
namespace ast
{
	class Arguments : public Node
	{
		private:
			shptr<vec<shptr<Expression>>> arguments;
		public:
			Arguments();
			Arguments(shptr<vec<shptr<ast::Expression>>> arguments);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual shptr<vec<shptr<Expression>>> getArgumentExpressions() const;
			virtual void accept(ASTVisitor& visitor) const;
	};
}
#endif // ARGUMENTS_HPP
