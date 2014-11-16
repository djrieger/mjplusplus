#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include "Node.hpp"
#include "Expression.hpp"

#include <vector>
namespace ast
{
	class Arguments : public Node
	{
		private:
			std::unique_ptr<std::vector<std::unique_ptr<ast::Expression>>> arguments;
		public:
			Arguments();
			Arguments(std::unique_ptr<std::vector<std::unique_ptr<ast::Expression>>>& arguments);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}
#endif // ARGUMENTS_HPP
