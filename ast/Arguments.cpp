#include "Arguments.hpp"

ast::Arguments::Arguments(std::unique_ptr<std::vector<std::unique_ptr<ast::Ident>>>& arguments) : arguments(std::move(arguments))
{

}

std::string ast::Arguments::toString()
{
	std::string ret = "(";
	auto it = arguments->begin();

	while (1)
	{
		ret += (**it).toString();

		if (++it != arguments->end())
			ret += ", ";
		else
			break;
	}

	ret += ")";

	return ret;
}