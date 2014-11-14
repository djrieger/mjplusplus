#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include "Node.hpp"
#include "Ident.hpp"
#include <vector>
namespace ast
{
	class Arguments : public Node
	{
		private:
			std::unique_ptr<std::vector<std::unique_ptr<ast::Ident>>> arguments;
		public:
			Arguments(std::unique_ptr<std::vector<std::unique_ptr<ast::Ident>>>& arguments);
			virtual std::string toString() const;
	};
}
#endif // ARGUMENTS_HPP