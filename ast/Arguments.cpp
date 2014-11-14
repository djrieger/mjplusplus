#include "Arguments.hpp"

ast::Arguments::Arguments(std::unique_ptr<std::vector<std::unique_ptr<Expression> > >& arguments) : arguments(std::move(arguments))
{

}

std::string ast::Arguments::toString() const
{
	std::string ret = "(";

	for (auto it = arguments->begin(); it != arguments->end(); it++)
	{
		ret += (**it).toString();

		if (it + 1 != arguments->end())
			ret += ", ";
	}

	ret += ")";

	return ret;
}
