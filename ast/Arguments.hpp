#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include "Node.hpp"
#include "Ident.hpp"
#include <vector>

class Arguments : public Node
{
	private:
		std::unique_ptr<std::vector<std::uniqe_ptr<ast::Ident>>> arguments;
	public:
		Arguments(std::unique_ptr<std::vector<std::uniqe_ptr<ast::Ident>>>& arguments);
		virtual std::string toString();
};

#endif // ARGUMENTS_HPP