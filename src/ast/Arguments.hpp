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
			uptr<vec<uptr<ast::Expression>>> arguments;
		public:
			Arguments();
			Arguments(uptr<vec<uptr<ast::Expression>>> arguments);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}
#endif // ARGUMENTS_HPP
